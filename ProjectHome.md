## About ##

PRec is a panorama recognizer that I wrote while working at Intel Research Lab at the faculty of Computational Mathematics and Cybernetics of the Moscow State University.

PRec automatically recognizes panoramas in a set of input images and stitches them. The process used is described in `[1]`. PRec is written in C++ and uses Intel Integrated Performance Primitives library for low-level image operations.

The overview of the process is as follows:
  * SIFT features are extracted from input images as described in `[2]`.
  * SIFT features are matched using fast approximate nearest neighbor search as described in `[3]`.
  * For each pair of input images, a RANSAC-class algorithm is used to filter out outlying feature matches.
  * Image matches are filtered and a set of panoramas is constructed.
  * For each panorama, bundle adjustment is used to jointly optimize camera parameters of input images.

`[1]` M. Brown and D. G. Lowe. Recognising Panoramas. International Conference on Computer Vision, 2003. <a href='http://lcav.epfl.ch/~brown/papers/iccv2003.pdf'><code>[PDF]</code></a>.<br />
`[2]` D. G. Lowe. Distinctive Image Features from Scale-Invariant Keypoints. International Journal of Computer Vision, 2004. <a href='http://www.cs.ubc.ca/~lowe/papers/ijcv04.pdf'><code>[PDF]</code></a>.<br />
`[3]` J. S. Beis and D. G. Lowe. Shape indexing using approximate nearest-neighbour search in high-dimensional spaces. Conference on Computer Vision and Pattern Recognition, 1997. <a href='http://www.cs.ubc.ca/~lowe/papers/cvpr97.pdf'><code>[PDF]</code></a>.


## Examples ##
<table border='1px'>
<tr><td align='center'>
<a href='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano1.jpg'><img src='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano1-150x112.jpg' alt='' title='Pano 1' width='150' height='112' /></a><a href='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano2.jpg'><img src='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano2-150x112.jpg' alt='' title='Pano 2' width='150' height='112' /></a><a href='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano3.jpg'><img src='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano3-150x112.jpg' alt='' title='Pano 3' width='150' height='112' /></a><a href='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano4.jpg'><img src='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/pano4-150x112.jpg' alt='' title='Pano 4' width='150' height='112' /></a><br />Example input.<br>
</td></tr>
<tr><td align='center'>
<a href='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/panoresult.jpg'><img src='https://wp-elric-ru.googlecode.com/svn/trunk/content/images/panoresult-300x78.jpg' alt='' title='PRec example output' width='300' height='78' /></a><br />Example output.<br>
</td></tr>
</table>