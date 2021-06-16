

### StackProf

#### Goal

The main goal of stackprof is to compute stacked profiles on displacement maps (with two components or just one) and provide graphic view on profiles to perform measurements. Its main target is to make measurements along seismic fault.

#### Main features

- Flexible view of input images or displacement maps: adjustable window size, move/zoom/center on a point, etc.
- Edit one or more traces over an image or a displacement map
- Add oriented stacked profile boxes along a trace using automatic distributions. Two methods available:
  - Boxes as close to each other as possible
  - Set distance between box centers
- Add/move/resize/remove any oriented stacked profile boxes along a trace
- Reversible profile orientation
- Possible usage of correlation score map to compute weighted mean or weighted median
- Filter values according to a threshold on correlation score map
- Flexible view of computed stacked profiles curves: 
  - adjustable window size
  - up to three computed components displayed at the same time (Perpendicular, Parallel, deltaZ), sharing same y-axis range or not.
- Adjustable profile center position for the offset measurement (in addition to left and right sides around the profile center)
- Geo referencing: 
  - converts trace to match project if required
  - displays and exports WGS84 locations
- Export results in ASCII and json

With:

- up to 100 000 x 100 000 pixels input images
- get nearest pixel or 2x2 square of surrounding pixels to get pixel values for computation
- Set pixel value conversion factor ('MicMac step value' and spatial resolution)
- Possible exponentiation from 2 to 6 for weight to compute weighted mean or weighted median
- Adjustable color and width of computed stacked profiles curves items
- Flag any profile to filter them at export or other purpose
- Edit and export trace from project

