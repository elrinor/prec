#ifndef __ARX_KDTREE_H__
#define __ARX_KDTREE_H__

#include <limits>
#include <algorithm>
#include <memory>
#include <cassert>
#include "smart_ptr.h"
#include "Collections.h"
#include "Utility.h"

namespace arx {
  namespace detail {
    /**
     * Sorted list of fixed size that stores only the smallest items.
     */
    template<class T>
    class KSortedList {
    public:
      typedef std::size_t size_type;
      typedef T value_type;

    private:
      value_type* impl;
      value_type* begin;
      size_type myMaxSize;
      size_type count;

      value_type& get(size_type index) {
        return *(this->begin + index);
      }

      KSortedList& operator= (const KSortedList&);
      KSortedList(const KSortedList&);

    public:
      KSortedList() {}
      KSortedList(size_type maxSize) {
        assert(maxSize > 0);
        this->myMaxSize = maxSize;
        this->impl = new value_type[maxSize];
        this->count = 0;
        this->begin = this->impl;
      }
      ~KSortedList() {
        delete[] this->impl;
      }
      const value_type& operator[] (size_type index) const {
        return *(this->begin + index);
      }
      size_type size() const {
        return this->count;
      }
      size_type maxSize() const {
        return this->myMaxSize;
      }
      void add(value_type elem) {
        size_type pos = this->count;
        if(pos == this->myMaxSize) {
          if(!(this->get(pos - 1) < elem))
            pos--;
        } else
          this->count++;
        while(pos > 0 && !(this->get(pos - 1) < elem)) {
          this->get(pos) = this->get(pos - 1);
          pos--;
        }
        if(pos < this->myMaxSize)
          this->get(pos) = elem;
      }
      void slide() {
        this->begin++;
        this->myMaxSize--;
        this->count--;
      }
      value_type* getImpl() {
        return this->impl;
      }
    };

    /** 
     * Super template is used to determine the type for intermediate calculations. 
     */
    template<class T> struct Super {
      typedef T type;
    };
#define ARX_DEFINE_SUPERTYPE(FORTYPE, SUPERTYPE)                                \
    template<> struct Super<FORTYPE> {                                          \
      typedef SUPERTYPE type;                                                   \
    };                                                                          
    ARX_DEFINE_SUPERTYPE(char,           int)
    ARX_DEFINE_SUPERTYPE(unsigned char,  int)
    ARX_DEFINE_SUPERTYPE(short,          int)
    ARX_DEFINE_SUPERTYPE(unsigned short, int)
    ARX_DEFINE_SUPERTYPE(int,            long long)
    ARX_DEFINE_SUPERTYPE(unsigned int,   long long)
    ARX_DEFINE_SUPERTYPE(long,           long long)
    ARX_DEFINE_SUPERTYPE(unsigned long,  long long)
#undef ARX_DEFINE_SUPERTYPE

    /** 
     * KDTree implementation 
     */
    template<class VecT, int dim, class ElemT, class ElemSuperT> class KDTreeImpl {
    public:

      /**
       * PointEntry stores a point and a squared distance to it. List search returns an ArrayList of PointEntries.
       */
      class PointEntry {
      private:
        VecT elem;
        ElemSuperT distSqr;
      public:
        PointEntry(const VecT& elem, ElemSuperT distSqr): elem(elem), distSqr(distSqr) {};
        PointEntry() {};
        ElemSuperT getDistSqr() const {
          return this->distSqr;
        }
        const VecT& getElem() const {
          return this->elem;
        }
        bool operator< (const PointEntry& that) const {
          return this->distSqr < that.distSqr;
        }
      };

      static ElemSuperT distanceSqr(const VecT& x, const VecT& y) {
        ElemSuperT result = 0;
        for(int i = 0; i < dim; i++)
          result += sqr(static_cast<ElemSuperT>(x[i]) - static_cast<ElemSuperT>(y[i]));
        return result;
      }

      typedef ArrayList<PointEntry> PointList;
      typedef typename VecT::size_type dim_type;
      typedef std::size_t size_type;

    private:
      class KDTreeNode;

      /**
       * HyperRect represents a bounding rectangle in a point space. It also stores a distance to the given point.
       */
      class HyperRect {
      private:
        // this works as a [...) interval!
        ElemT min[dim], max[dim];
        ElemSuperT distSqr;

      public:
        HyperRect() {};

        static HyperRect createInfiniteRect() {
          HyperRect result;
          for(int i = 0; i < dim; i++) {
            result.min[i] = std::numeric_limits<ElemT>::min();
            result.max[i] = std::numeric_limits<ElemT>::max();
          }
          result.distSqr = 0;
          return result;
        }

        void splitTo(dim_type splitDim, ElemT splitVal, const VecT& point, HyperRect* that) {
          ElemSuperT dimDistSqr;
          if(point[splitDim] <= this->min[splitDim])
            dimDistSqr = sqr(static_cast<ElemSuperT>(min[splitDim]) - static_cast<ElemSuperT>(point[splitDim]));
          else if(point[splitDim] >= this->max[splitDim])
            dimDistSqr = sqr(static_cast<ElemSuperT>(max[splitDim]) - static_cast<ElemSuperT>(point[splitDim]));
          else
            dimDistSqr = 0;

          *that = *this;
          this->max[splitDim] = splitVal;
          that->min[splitDim] = splitVal;

          if(point[splitDim] > splitVal)
            this->distSqr += -dimDistSqr + sqr(static_cast<ElemSuperT>(point[splitDim]) - static_cast<ElemSuperT>(splitVal));
          else
            that->distSqr += -dimDistSqr + sqr(static_cast<ElemSuperT>(point[splitDim]) - static_cast<ElemSuperT>(splitVal));
        }

        ElemSuperT getDistSqr() {
          return this->distSqr;
        }
      };

      struct BBFEntry {
        ElemSuperT dist;
        HyperRect* rect;
        const KDTreeNode* node;

        BBFEntry(HyperRect* rect, const KDTreeNode* node, ElemSuperT dist): rect(rect), node(node), dist(dist) {}
        BBFEntry() {}

        bool operator< (const BBFEntry& that) const {
          return this->dist < that.dist;
        }
      };

      struct BBFContext {
        const VecT& point;
        ElemSuperT maxDistSqr;
        KSortedList<PointEntry> result;
        KSortedList<BBFEntry> searchList;
        ArrayList<HyperRect> rects;
        int rectPos;
        int stepsLeft;

        BBFContext(const VecT& point, int k, int maxSteps): point(point), maxDistSqr(std::numeric_limits<ElemSuperT>::max()), stepsLeft(maxSteps), result(k), searchList(maxSteps) {
          this->rects.resize(maxSteps + 1);
          this->rects[0] = HyperRect::createInfiniteRect();
          this->rectPos = 1;
        }
      };

      /**
       * Single node of a kd-tree.
       */
      class KDTreeNode {
      private:
        VecT elem;
        dim_type splitDim;
        KDTreeNode *left, *right;

      public:
        KDTreeNode(const VecT& elem): elem(elem) {};

        template<class ArrayOfVecT>
        static void placeKI(ArrayOfVecT& points, dim_type sortDim, typename ArrayOfVecT::size_type lo, typename ArrayOfVecT::size_type hi, typename ArrayOfVecT::size_type k) {
          typedef typename ArrayOfVecT::size_type size_type;
          if(lo >= hi)
            return;
          ElemT x = points[k][sortDim];
          size_type l = lo;
          size_type h = hi;
          do {
            while(points[l][sortDim] < x) l++;
            while(points[h][sortDim] > x) h--;
            if(l <= h) {
              std::swap(points[l], points[h]);
              l++;
              h--;
            }
          } while (l <= h);

          if(k <= h)
            placeKI(points, sortDim, lo, h, k);
          else if (k >= l)
            placeKI(points, sortDim, l, hi, k);
        }

        template<class ArrayOfVecT>
        static const VecT& placeK(ArrayOfVecT& points, dim_type sortDim, typename ArrayOfVecT::size_type k) {
          placeKI(points, sortDim, 0, static_cast<int>(points.size()) - 1, k);
          return points[k];
        }

        template<class ArrayOfVecT>
        static const VecT& findSplitPoint(ArrayOfVecT& points, dim_type& outSplitDim) {
          typedef typename ArrayOfVecT::size_type size_type;
          ElemT min[dim], max[dim];
          for(size_type i = 0; i < dim; i++) {
            min[i] = std::numeric_limits<ElemT>::max();
            max[i] = std::numeric_limits<ElemT>::min();
          }
          for(size_type i = 0; i < points.size(); i++) {
            for(dim_type j = 0; j < dim; j++) {
              ElemT val = points[i][j];
              if(val > max[j])
                max[j] = val;
              if(val < min[j])
                min[j] = val;
            }
          }
          dim_type maxDiffDim;
          ElemSuperT maxDiff = 0;
          for(dim_type i = 0; i < dim; i++) {
            ElemSuperT diff = max[i] - min[i];
            if(diff >= maxDiff) {
              maxDiff = diff;
              maxDiffDim = i;
            }
          }
          outSplitDim = maxDiffDim;
          // The splitting dimension is maxDiffDim.
          // Then we have to find a split point.
          return placeK(points, maxDiffDim, points.size() / 2);
          // TODO: maybe it'd be better to use middle point, not the median
        }

        template<class ArrayOfVecT, template<class> class SliceStoreMode>
        static KDTreeNode* buildNode(Slice<ArrayOfVecT, SliceStoreMode>& points) {
          if(points.size() == 0)
            return NULL;
          dim_type splitDim;
          KDTreeNode* result = new KDTreeNode(findSplitPoint(points, splitDim));
          result->splitDim = splitDim;
          result->left  = buildNode(createSlice<SliceStoreMode>(points, 0, points.size() / 2)); 
          result->right = buildNode(createSlice<SliceStoreMode>(points, points.size() / 2 + 1, points.size())); 
          return result;
        }

        VecT nearestNeighbour(const VecT& point, HyperRect* leftRect, ElemSuperT maxDistSqr, ElemSuperT& outDistSqr) const {
          HyperRect rightRect;
          leftRect->splitTo(this->splitDim, this->elem[this->splitDim], point, &rightRect);

          HyperRect* nearRect;
          HyperRect* farRect;
          KDTreeNode* nearNode;
          KDTreeNode* farNode;
          if(point[this->splitDim] <= this->elem[this->splitDim]) {
            nearRect = leftRect;
            farRect = &rightRect;
            nearNode = this->left;
            farNode = this->right;
          } else {
            nearRect = &rightRect;
            farRect = leftRect;
            nearNode = this->right;
            farNode = this->left;
          }

          ElemSuperT distSqr;
          VecT nearest;

          if(nearNode == NULL)
            distSqr = std::numeric_limits<ElemSuperT>::max();
          else
            nearest = nearNode->nearestNeighbour(point, nearRect, maxDistSqr, distSqr);

          maxDistSqr = min(distSqr, maxDistSqr);

          if(farRect->getDistSqr() < maxDistSqr) {
            ElemSuperT thisDistSqr = distanceSqr(this->elem, point);
            if(thisDistSqr < distSqr) {
              nearest = this->elem;
              distSqr = thisDistSqr;
              maxDistSqr = thisDistSqr;
            }

            ElemSuperT newDistSqr;
            VecT newNearest;
            if(farNode == NULL)
              newDistSqr = std::numeric_limits<ElemSuperT>::max();
            else
              newNearest = farNode->nearestNeighbour(point, farRect, maxDistSqr, newDistSqr);

            if(newDistSqr < distSqr) {
              nearest = newNearest;
              distSqr = newDistSqr;
            }
          }

          outDistSqr = distSqr;
          return nearest;
        }

        void nearestNeighbourList(const VecT& point, HyperRect* leftRect, KSortedList<PointEntry>* best, ElemSuperT maxDistSqr) const {
          best->add(PointEntry(this->elem, distanceSqr(this->elem, point)));

          HyperRect rightRect;
          leftRect->splitTo(this->splitDim, this->elem[this->splitDim], point, &rightRect);

          HyperRect* nearRect;
          HyperRect* farRect;
          KDTreeNode* nearNode;
          KDTreeNode* farNode;
          if(point[this->splitDim] <= this->elem[this->splitDim]) {
            nearRect = leftRect;
            farRect = &rightRect;
            nearNode = this->left;
            farNode = this->right;
          } else {
            nearRect = &rightRect;
            farRect = leftRect;
            nearNode = this->right;
            farNode = this->left;
          }

          if(nearNode != NULL)
            nearNode->nearestNeighbourList(point, nearRect, best, maxDistSqr);
          if(best->size() >= best->maxSize())
            maxDistSqr = (*best)[best->maxSize() - 1].getDistSqr();
          if(farNode != NULL && farRect->getDistSqr() < maxDistSqr)
            farNode->nearestNeighbourList(point, farRect, best, maxDistSqr);
        }

        void nearestNeighbourListBBF(BBFContext* context, HyperRect* leftRect) const {
          context->result.add(PointEntry(this->elem, distanceSqr(this->elem, context->point)));

          if(context->stepsLeft <= 0)
            return;
          context->stepsLeft--;

          HyperRect* rightRect = &context->rects[context->rectPos++];
          leftRect->splitTo(this->splitDim, this->elem[this->splitDim], context->point, rightRect);

          HyperRect* nearRect;
          HyperRect* farRect;
          KDTreeNode* nearNode;
          KDTreeNode* farNode;
          if(context->point[this->splitDim] <= this->elem[this->splitDim]) {
            nearRect = leftRect;
            farRect = rightRect;
            nearNode = this->left;
            farNode = this->right;
          } else {
            nearRect = rightRect;
            farRect = leftRect;
            nearNode = this->right;
            farNode = this->left;
          }

          context->searchList.add(BBFEntry(farRect, farNode, farRect->getDistSqr()));
          if(nearNode != NULL) // it seems we don't need the isWithinRange check here
            nearNode->nearestNeighbourListBBF(context, nearRect);
          if(context->result.size() >= context->result.maxSize())
            context->maxDistSqr = context->result[context->result.maxSize() - 1].getDistSqr();
          if(context->searchList.size() > 0) {
            BBFEntry entry = context->searchList[0];
            context->searchList.slide();
            if(entry.node != NULL && entry.rect->getDistSqr() < context->maxDistSqr)
              entry.node->nearestNeighbourListBBF(context, entry.rect);
          }
        }

      };

      KDTreeNode *root;
      size_type mySize;


    public:
      template<class ArrayOfVecT, template<class> class SliceStoreMode>
      static KDTreeImpl* buildTree(Slice<ArrayOfVecT, SliceStoreMode>& points) {
        KDTreeImpl* result = new KDTreeImpl();
        result->mySize = points.size();
        result->root = KDTreeNode::buildNode(points);
        return result;
      }

      VecT nearestNeighbour(const VecT& point, ElemSuperT& outDistSqr) const {
        HyperRect rect = HyperRect::createInfiniteRect();
        return this->root->nearestNeighbour(point, &rect, std::numeric_limits<ElemSuperT>::max(), outDistSqr);
      }

      PointList nearestNeighbourList(const VecT& point, int k) const {
        HyperRect rect = HyperRect::createInfiniteRect();
        KSortedList<PointEntry> sortedList(k);
        this->root->nearestNeighbourList(point, &rect, &sortedList, std::numeric_limits<ElemSuperT>::max());
        PointList result;
        result.resize(sortedList.size());
        PointList::value_type* impl = sortedList.getImpl();
        for(unsigned int i = 0; i < sortedList.size(); i++)
          result[i] = impl[i];
        return result;
      }

      PointList nearestNeighbourListBBF(const VecT& point, int k, unsigned int maxSteps) const {
        scoped_ptr<BBFContext> context(new BBFContext(point, k, maxSteps));
        this->root->nearestNeighbourListBBF(context.get(), &context->rects[0]);
        PointList result;
        result.resize(context->result.size());
        PointList::value_type* impl = context->result.getImpl();
        for(unsigned int i = 0; i < context->result.size(); i++)
          result[i] = impl[i];
        return result;
      }

      size_type size() const {
        return this->mySize;
      }
    };

  } // namespace detail


  /**
   * KDTree class has a reference-counted pointer semantics.
   * @param VecT type of a point in KDTree. It is stored locally in subroutines and in KDTree, that's why it'd better be a pointer or a class with a pointer semantics.
   *   VecT must be indexable, i.e. must define operator[].
   * @param dim the dimensionality of a point. It can be guessed automatically, if you define VecT::static_size.
   * @param ElemT type of a point's element. Can be guessed automatically if VecT is a pointer type or if VecT::value_type is defined.
   * @param ElemSuperT a type for intermediate calculations. Defaults to int for types that are smaller than int.
   */
  template<class VecT, int dim = VecT::static_size, class ElemT = typename ValueType<VecT>::type, class ElemSuperT = typename detail::Super<ElemT>::type> class KDTree {
  private:
    typedef detail::KDTreeImpl<VecT, dim, ElemT, ElemSuperT> KDTreeImpl;
    shared_ptr<KDTreeImpl> tree;
    KDTree(KDTreeImpl* tree): tree(tree) {}

  public:
    typedef KDTree<VecT, dim, ElemT, ElemSuperT> this_type;
    typedef typename KDTreeImpl::size_type size_type;
    typedef typename KDTreeImpl::PointEntry PointEntry;
    typedef typename KDTreeImpl::PointList PointList;

    KDTree() {}

    /**
     * Factory method for creating KDTrees. Please note that this method modifies points array, rearranging its items.
     * @param points array of points which are used for KDTree construction. 
     */
    template<class ArrayOfVecT>
    static this_type buildTree(ArrayOfVecT& points) {
      return KDTree(KDTreeImpl::buildTree(createRefSlice(points, 0, points.size())));
    }

    /**
     * Find an exact nearest neighbor of point in KDTree.
     * @param point a point to search a nearest neighbor for.
     * @param (out) outDistSqr a squared euclidean distance to the nearest neighbor.
     * @returns a nearest neighbor.
     */
    VecT nearestNeighbour(const VecT& point, ElemSuperT& outDistSqr) const {
      return tree->nearestNeighbour(point, outDistSqr);
    }

    /**
     * Find k exact nearest neighbors for point.
     * @param point a point to search a nearest neighbors for.
     * @param k a number of exact nearest neighbors to find.
     * @returns an ArrayList of PointEntry containing the nearest neighbors.
     */
    PointList nearestNeighbourList(const VecT& point, int k) const {
      return tree->nearestNeighbourList(point, k);
    }

    /**
     * Find k approximate nearest neighbors for point using Best-Bin-First algorithm with fixed number of iterations.
     * @param point a point to search a nearest neighbors for.
     * @param k a number of approximate nearest neighbors to find.
     * @param maxSteps a maximal number of BBF iterations.
     * @returns an ArrayList of PointEntry containing the nearest neighbors.
     */
    PointList nearestNeighbourListBBF(const VecT& point, int k, unsigned int maxSteps) const {
      return tree->nearestNeighbourListBBF(point, k, maxSteps);
    }

    /**
     * @returns the number of points in KDTree
     */
    size_type size() const {
      return tree->size();
    }

    /**
     * @returns a good estimation for number of iterations needed in BBF search for this KDTree.
     */
    unsigned int estimateGoodBBFSearchDepth() const {
      // TODO: It seems this BlAcK mAgIcK sPeLl needs deeper investigation...
      return (unsigned int) max(130.0f, (log(static_cast<float>(this->size())) / log(1000.0f)) * 130.0f);
    }
  };

}

#endif
