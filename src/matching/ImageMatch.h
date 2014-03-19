#ifndef __IMAGEMATCH_H__
#define __IMAGEMATCH_H__

#include "config.h"
#include "arx/smart_ptr.h"
#include "arx/Collections.h"
#include "PanoImage.h"
#include "Match.h"
#include "ImageMatchModel.h"

namespace prec {
// -------------------------------------------------------------------------- //
// ImageMatch
// -------------------------------------------------------------------------- //
  class ImageMatch {
  private:
    struct PanoImageMatchData {
      arx::ArrayList<Match> matches;

      ImageMatchModel matchModel;

      arx::array<PanoImage, 2> images;

      PanoImageMatchData(PanoImage image0, PanoImage image1) {
        if(image0.getId() > image1.getId()) {
          images[0] = image0;
          images[1] = image1;
        } else {
          images[0] = image1;
          images[1] = image0;
        }
      }
    };
    
    arx::shared_ptr<PanoImageMatchData> data;

  public:
    ImageMatch() {}

    ImageMatch(PanoImage image0, PanoImage image1): data(new PanoImageMatchData(image0, image1)) {}

    void setMatches(const arx::ArrayList<Match>& matches) { data->matches = matches; }
    const arx::ArrayList<Match>& getMatches() const { return data->matches; }
    arx::ArrayList<Match>& getMatches() { return data->matches; }
    const Match& getMatch(size_t index) const { return data->matches[index]; }

    void setMatchModel(const ImageMatchModel& matchModel) { data->matchModel = matchModel; }
    const ImageMatchModel& getMatchModel() const { return data->matchModel; }

    const PanoImage& getPanoImage(int index) const { return data->images[index]; }

    bool isNull() const { return data.get() == NULL; }
  };

} // namespace prec

#endif // __IMAGEMATCH_H__
