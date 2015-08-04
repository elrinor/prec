#About

PRec is a panorama recognizer that I wrote while working at Intel Research Lab at the faculty of Computational Mathematics and Cybernetics of the Moscow State University.

PRec automatically recognizes panoramas in a set of input images and stitches them. The process used is described in [1]. PRec is written in C++ and uses Intel Integrated Performance Primitives library for low-level image operations.

The overview of the process is as follows:
  * SIFT features are extracted from input images as described in [2]. 
  * SIFT features are matched using fast approximate nearest neighbor search as described in [3].
  * For each pair of input images, a RANSAC-class algorithm is used to filter out outlying feature matches.
  * Image matches are filtered and a set of panoramas is constructed.
  * For each panorama, bundle adjustment is used to jointly optimize camera parameters of input images.

[1] M. Brown and D. G. Lowe. Recognising Panoramas. International Conference on Computer Vision, 2003. <a href="http://lcav.epfl.ch/~brown/papers/iccv2003.pdf">[PDF]</a>.<br/>
[2] D. G. Lowe. Distinctive Image Features from Scale-Invariant Keypoints. International Journal of Computer Vision, 2004. <a href="http://www.cs.ubc.ca/~lowe/papers/ijcv04.pdf">[PDF]</a>.<br/>
[3] J. S. Beis and D. G. Lowe. Shape indexing using approximate nearest-neighbour search in high-dimensional spaces. Conference on Computer Vision and Pattern Recognition, 1997. <a href="http://www.cs.ubc.ca/~lowe/papers/cvpr97.pdf">[PDF]</a>.
