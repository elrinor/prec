#include "config.h"
#include <cmath>
#include <cassert>
#include <arx/Collections.h>
#include <arx/KDTree.h>
#include "Matcher.h"
#include "Ransac.h"
#include "ImageMatchModel.h"

using namespace std;
using namespace arx;

namespace prec {
  namespace detail {
// -------------------------------------------------------------------------- //
// MatcherImpl
// -------------------------------------------------------------------------- //
    class MatcherImpl {
    private:
      unsigned int minimumMatches;
      unsigned int maximumMatches;
      bool useRANSAC;

      typedef RANSAC<ImageMatchModel> RANSAC;
      typedef arx::Map<arx::UnorderedPair<int>, ImageMatch> MatchMap;
      typedef arx::Set<arx::UnorderedPair<SIFT, SIFTPtrComparer> > KeyPointPairSet; // TODO: hash_set may be faster

      /**
       * Find the best match for the given SIFT key in the given KDTree tree.
       *
       * @param key keypoint to match
       * @param tree kd-tree containing keypoints that will be matched against key
       * @param searchDepth depth for BBF kd-tree search
       * @returns the best match in the kd-tree for the given keypoint. If the 
       *   match found is non-distinctive, returns NULL Match.
       */
      static Match match(const SIFT key, const KDTree<SIFT> tree, unsigned int searchDepth) {
        KDTree<SIFT>::PointList nnList = tree.nearestNeighbourListBBF(key, 3, searchDepth);
        KDTree<SIFT>::PointEntry e0 = nnList[0];
        KDTree<SIFT>::PointEntry e1 = nnList[1];

        /* First match may be with the keypoint itself */
        if(e0.getElem() == key) {
          e0 = e1;
          e1 = nnList[2];
        }

        /* Skip non-distinctive matches */
        if(static_cast<float>(e0.getDistSqr()) / e1.getDistSqr() > sqr(0.8f))
          return Match(); /* i.e. return NULL */

        return Match(key, e0.getElem(), e0.getDistSqr());
      }

      void addToComponent(int n, Map<int, ArrayList<int> > graph, ArrayList<int> nodes, ArrayList<UnorderedPair<int> > edges, Set<int> used) {
        assert(!used.contains(n) && graph.contains(n));
        
        /* Mark as used. */
        used.insert(n);

        /* Add node. */
        nodes.add(n);
        
        /* Add all edges. */
        for(size_t i = 0; i < graph[n].size(); i++)
          if(!used.contains(graph[n][i]))
            edges.add(make_upair(n, graph[n][i]));
        
        /* Then recurse. */
        for(size_t i = 0; i < graph[n].size(); i++) 
          if(!used.contains(graph[n][i]))
            addToComponent(graph[n][i], graph, nodes, edges, used);
      }

      ArrayList<pair<ArrayList<int>, ArrayList<UnorderedPair<int> > > > splitIntoConnectedComponents(Map<int, ArrayList<int> > graph) {
        ArrayList<pair<ArrayList<int>, ArrayList<UnorderedPair<int> > > > result;
        
        Set<int> used;

        for(Map<int, ArrayList<int> >::const_iterator i = graph.begin(); i != graph.end(); i++) {
          if(!used.contains(i->first)) {
            ArrayList<int> nodes;
            ArrayList<UnorderedPair<int> > edges;
            addToComponent(i->first, graph, nodes, edges, used);
            result.push_back(make_pair(nodes, edges));
          }
        }

        return result;
      }

      ArrayList<Panorama> splitIntoPanoramas(const ArrayList<PanoImage> imageList, MatchMap matchMap) {
        ArrayList<Panorama> result;

        /* Create (Id -> Image) map. */
        Map<int, PanoImage> imageMap;
        for(size_t i = 0; i < imageList.size(); i++)
          imageMap[imageList[i].getId()] = imageList[i];

        /* Create graph representation. */
        Map<int, ArrayList<int> > graph;

        /* Create nodes. */
        for(size_t i = 0; i < imageList.size(); i++)
          graph[imageList[i].getId()];

        /* Add edges. */
        for(MatchMap::const_iterator i = matchMap.begin(); i != matchMap.end(); i++) {
          graph[i->first.first].push_back(i->first.second);
          graph[i->first.second].push_back(i->first.first);
        }

        /* Split into connected components to form panoramas. */
        ArrayList<pair<ArrayList<int>, ArrayList<UnorderedPair<int> > > > components = splitIntoConnectedComponents(graph);
        for(size_t i = 0; i < components.size(); i++) {
          Panorama p;
          
          /* Add images into panorama. */
          ArrayList<int> nodes = components[i].first;
          for(size_t j = 0; j < nodes.size(); j++)
            p.addImage(imageMap[nodes[j]]);
          
          /* Add matches into panorama. */
          ArrayList<UnorderedPair<int> > edges = components[i].second;
          for(size_t j = 0; j < edges.size(); j++)
            p.addMatch(matchMap[edges[j]]);

          /* Prepare result. */
          result.push_back(p);
        }
        
        return result;
      }


    public:
      /**
       * Constructor.
       * 
       * @param minimumMatches Minimum number of matches required in final result
       * @param maximumMatches Number of best matches to keep, or zero to keep all
       * @useRANSAC Use RANSAC filtering?
       */
      MatcherImpl(unsigned int minimumMatches, unsigned int maximumMatches, bool useRANSAC):
        minimumMatches(minimumMatches), maximumMatches(maximumMatches), useRANSAC(useRANSAC) {
        assert(minimumMatches <= maximumMatches);
      }

      /**
       * @param imageList List of imageList to match
       * @returns List of panoramas found
       */
      ArrayList<Panorama> matchImages(ArrayList<PanoImage> imageList) {
        MatchMap matchMap; /* (<Id, Id> -> Match) map */

        /* Build global keypoint list. */
        ArrayList<SIFT> allKeysList;
        for(unsigned int i = 0; i < imageList.size(); i++)
          allKeysList.insert(allKeysList.end(), imageList[i].getKeyPointList().begin(), imageList[i].getKeyPointList().end());

        /* Build global KDTree. */
        KDTree<SIFT> kdTree = KDTree<SIFT>::buildTree(allKeysList);

        /* Check KDTree size. */
        if(kdTree.size() < maximumMatches * 2)
          return splitIntoPanoramas(imageList, matchMap);

        /* Workaround for reverse matches. */
        KeyPointPairSet used;
        
        /* Estimate search depth. */
        int searchDepth = kdTree.estimateGoodBBFSearchDepth();

        /* Fill matchMap. */
        for(ArrayList<PanoImage>::iterator i = imageList.begin(); i != imageList.end(); i++)
          for(ArrayList<PanoImage>::iterator j = i + 1; j != imageList.end(); j++)
            matchMap.insert(make_pair(make_upair(i->getId(), j->getId()), ImageMatch(*i, *j)));

        /* Match everything. */
        for(unsigned int i = 0; i < allKeysList.size(); i++) {
          Match m = match(allKeysList[i], kdTree, searchDepth);

          /* If no match found - continue. */
          if(m.isNull())
            continue;

          /* Skip self-matches. */
          if(m.getKey(0).getTag() == m.getKey(1).getTag())
            continue;

          /* Skip reverse matches. */
          KeyPointPairSet::iterator pos = used.find(m.getKeys());
          if(pos != used.end())
            continue;
          else
            used.insert(pos, m.getKeys());

          /* Add to list. */
          matchMap[make_upair(m.getKey(0).getTag(), m.getKey(1).getTag())].getMatches().add(m);
        }

        /* Filter ImageMatches. */
        for(MatchMap::iterator i = matchMap.begin(), nextI = i; (nextI != matchMap.end()) ? (nextI++, true) : false; i = nextI) {
          ImageMatch im = i->second;

          /* Ignore matchsets with less than minimumMatches matches. */
          if(im.getMatches().size() < minimumMatches) {
            matchMap.erase(i);
            continue;
          }

          /* For RANSAC we need at least one match pair plus one for verification */
          if(useRANSAC && im.getMatches().size() >= 3) {
            /* Create RANSAC algorithm processor */
            RANSAC ransac(2, minimumMatches);

            /* Fit RANSAC. */
            ImageMatchModel model;
            if(!ransac.fit(model, im.getMatches(), 0.5f, 0.95f, sqr(0.01f))) { // TODO: magic numbers?
              matchMap.erase(i);
              continue;
            }

            /* Overwrite matches with RANSAC checked ones. */
            im.setMatchModel(model);
            im.setMatches(model.getInliers());

/*
            Image3f result(2000, 2000);
            result.fill(Color3f(0));

            PanoImage im0 = im.getPanoImage(0), im1 = im.getPanoImage(1);

            Matrix3f toCenter = Matrix3f::translation(1000, 1000);
            
            result.draw(im0.getOriginal(), 
              toCenter * 
              Matrix3f::translation(-im0.getOriginal().getWidth() / 2.0f, -im1.getOriginal().getHeight() / 2.0f));

            result.draw(im1.getOriginal(), 
              toCenter * 
              Matrix3f::scale(1.0f / im1.getKeyPointScaleFactor()) *
              im.getMatchModel().getAffineTransform() * 
              Matrix3f::scale(im1.getKeyPointScaleFactor()) *
              Matrix3f::translation(-im1.getOriginal().getWidth() / 2.0f, -im1.getOriginal().getHeight() / 2.0f));
            result.saveToFile("result.jpg");

            */
          }

          // TODO: Further filter matches

          /* Leave only maximumMatches best matches */
          if(maximumMatches != 0 && im.getMatches().size() > maximumMatches) {
            nth_element(im.getMatches().begin(), im.getMatches().begin() + (maximumMatches - 1), im.getMatches().end(), MatchDistComparer());
            im.getMatches().erase(maximumMatches, im.getMatches().size());
          }
        }

        return splitIntoPanoramas(imageList, matchMap);
      }
      
  #if 0
      ArrayList<unsigned int> matchSingleImage(ArrayList<KeyPoint> keys, ArrayList<ArrayList<KeyPoint> > keyList) {
        /* Build kd-trees */
        ArrayList<KDTree<KeyPoint> > kdTrees;
        for(unsigned int i = 0; i < keyList.size(); i++)
          kdTrees.push_back(KDTree<KeyPoint>::buildTree(keyList[i]));

        /* Prepare result array */
        ArrayList<unsigned int> result;
        result.resize(keyList.size(), 0);

        /* Match keys
         * Outer cycle rolls through kd-trees, and inner one uses only one kd-tree.
         * This seem to be a more cache-efficient solution, compared to another
         * variant of loop nesting, even though both of them work fine. */
        for(unsigned int i = 0; i < kdTrees.size(); i++) {
          unsigned int searchDepth = kdTrees[i].estimateGoodBBFSearchDepth();
          for(unsigned int j = 0; j < keys.size(); j++)
            if(!match(keys[j], kdTrees[i], searchDepth).isNull())
              result[i]++;
        }

        return result;
      }

      
      ArrayList<unsigned int> matchSingleImage(Image3f image, ArrayList<Image3f> imageList) {
        SIFTExtractor extractor;
        ArrayList<KeyPoint> keys = extractor.extractKeyPoints(image);
        
        ArrayList<ArrayList<KeyPoint> > keyList;
        for(unsigned int i = 0; i < imageList.size(); i++)
          keyList.push_back(extractor.extractKeyPoints(imageList[i]));

        ArrayList<unsigned int> result = matchSingleImage(keys, keyList);

        /* Now we need to free all the keypoints we've allocated. */
        KeyPointDeleter::free(keys);
        KeyPointDeleter::free(keyList);

        return result;
      }

  #ifdef USE_CIPPIMAGE
      ArrayList<Image3f> toRGBImageArray(ArrayList<CIppImage*> images) {
        ArrayList<Image3f> result;
        for(unsigned int i = 0; i < images.size(); i++)
          result.push_back(Image3f(images[i]));
        return result;
      }

      ArrayList<unsigned int> matchSingleImage(CIppImage* image, ArrayList<CIppImage*> imageList) {
        return matchSingleImage(Image3f(image), toRGBImageArray(imageList));
      }
  #endif
      
  #endif
    };

  } // namespace detail

// -------------------------------------------------------------------------- //
// Matcher
// -------------------------------------------------------------------------- //
  Matcher::Matcher(unsigned int minimumMatches, unsigned int maximumMatches, bool useRANSAC): 
    impl(new detail::MatcherImpl(minimumMatches, maximumMatches, useRANSAC)) {}

  ArrayList<Panorama> Matcher::matchImages(ArrayList<PanoImage> images) {
    return impl->matchImages(images);
  }
  
#if 0

  ArrayList<unsigned int> Matcher::matchSingleImage(ArrayList<KeyPoint> keys, ArrayList<ArrayList<KeyPoint> > keyList) {
    return impl->matchSingleImage(keys, keyList);
  }

  ArrayList<unsigned int> Matcher::matchSingleImage(Image3f image, ArrayList<Image3f> imageList) {
    return impl->matchSingleImage(image, imageList);
  }

#ifdef USE_CIPPIMAGE
  arx::ArrayList<unsigned int> Matcher::matchSingleImage(CIppImage* image, arx::ArrayList<CIppImage*> imageList) {
    return impl->matchSingleImage(image, imageList);
  }
#endif

#endif
  
} // namespace prec
