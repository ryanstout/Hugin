// -*- c-basic-offset: 4 -*-
//
// Pablo d'Angelo <pablo.dangelo@web.de>
// Last change: Time-stamp: <26-Oct-2003 18:37:34 pablo@island.wh-wurm.uni-ulm.de>
//
//

#ifndef PANOIMAGE_H
#define PANOIMAGE_H

#include <iostream>
#include <vector>
#include <common/utils.h>
#include <common/math.h>
#include <vigra/diff2d.hxx>

namespace PT {

class Panorama;

/** optimization & stitching options. */
class ImageOptions {

public:
    ImageOptions()
    : featherWidth(10),
      ignoreFrameWidth(0),
      morph(false),
      docrop(false),
      autoCenterCrop(true),
      m_vigCorrMode(VIGCORR_NONE),
      active(true)
     { };

    // PT state
    /// u10           specify width of feather for stitching. default:10
    unsigned int featherWidth;
    /// m20           ignore a frame 20 pixels wide. default: 0
    unsigned int ignoreFrameWidth;

    /// Morph-to-fit using control points.
    bool morph;

	// crop parameters
    bool docrop;
    bool autoCenterCrop;
    vigra::Rect2D cropRect;

    /// vignetting correction mode (bitflags, no real enum)
    enum VignettingCorrMode { 
        VIGCORR_NONE = 0,      ///< no vignetting correction
        VIGCORR_RADIAL = 1,    ///< radial vignetting correction
        VIGCORR_FLATFIELD = 2, ///< flatfield correction
        VIGCORR_DIV = 4        ///< correct by division.
    };
    int m_vigCorrMode;
    // coefficients for vignetting correction (even degrees: 0,2,4,6, ...)
    std::string m_flatfield;

    // is image active (displayed in preview and used for optimisation)
    bool active;
};



/** description of a source pano image... 
 *
 *  In the long term, this simplified class will replace
 *  PanoImage and Image options and the variables array.
 */
class SrcPanoImage
{
public:

    enum Projection { RECTILINEAR = 0,
                      PANORAMIC = 1,
                      CIRCULAR_FISHEYE = 2,
                      FULL_FRAME_FISHEYE = 3,
                      EQUIRECTANGULAR = 4 };

    enum CropMode { NO_CROP=0,
                    CROP_RECTANGLE=1,
                    CROP_CIRCLE=2 };

    /// vignetting correction mode (bitflags, no real enum)
    enum VignettingCorrMode { 
        VIGCORR_NONE = 0,      ///< no vignetting correction
        VIGCORR_RADIAL = 1,    ///< radial vignetting correction
        VIGCORR_FLATFIELD = 2, ///< flatfield correction
        VIGCORR_DIV = 4        ///< correct by division.
    };

    SrcPanoImage()
    {
        setDefaults();
    }

    SrcPanoImage(const std::string &filename, vigra::Size2D size)
    {
        setDefaults();
        m_filename = filename;
        m_size = size ;
        m_cropRect = vigra::Rect2D(size);
    };

    void setDefaults()
    {
        m_proj = RECTILINEAR;
        m_hfov = 50;
        m_roll = 0;
        m_pitch = 0;
        m_yaw = 0;

        m_gamma = 1;

        m_radialDist.resize(4);
        for (unsigned i=0; i < 3; i++) {
            m_radialDist[i] = 0;
        }
        m_radialDist[3] = 1;

        m_crop = NO_CROP;

        m_vigCorrMode = VIGCORR_NONE;
        m_radialVigCorrCoeff.resize(4);
        m_radialVigCorrCoeff[0] = 1;
        for (unsigned i=1; i < 4; i++) {
            m_radialVigCorrCoeff[i] = 0;
        }

        m_ka.resize(3);
        for (unsigned i=0; i < 3; i++)
            m_ka[i] = 1;

        m_kb.resize(3);
        for (unsigned i=0; i < 3; i++)
            m_kb[i] = 0;

        m_lensNr = 0;
        m_featherWidth = 10;
        m_morph = false;
    }

    /** "resize" image,
     *  adjusts all distortion coefficients for usage with a source image
     *  of size @p size
     */
    void resize(const vigra::Size2D & size);


    /** check if a coordinate is inside the source image
     */
    bool isInside(vigra::Point2D p) const
    {
        switch(m_crop) {
            case NO_CROP:
            case CROP_RECTANGLE:
                return m_cropRect.contains(p);
            case CROP_CIRCLE:
            {
                if (0 > p.x || 0 > p.y || p.x >= m_size.x || p.y >= m_size.y) {
                    // outside image
                    return false;
                }
                FDiff2D cropCenter;
                cropCenter.x = m_cropRect.left() + m_cropRect.width()/2.0;
                cropCenter.y = m_cropRect.top() + m_cropRect.height()/2.0;
                double radius2 = std::min(m_cropRect.width()/2.0, m_cropRect.height()/2.0);
                radius2 = radius2 * radius2;
                FDiff2D pf = FDiff2D(p) - cropCenter;
                return (radius2 > pf.x*pf.x+pf.y*pf.y );
            }
        }
        // this should never be reached..
        return false;
    }

    bool horizontalWarpNeeded();

    // property accessors

    const std::string & getFilename() const
    { return m_filename; }
    void setFilename(const std::string & file)
    { m_filename = file; }

    const vigra::Size2D & getSize() const
    { return m_size; }
    void setSize(const vigra::Size2D & val)
    { m_size = val; }

    const Projection & getProjection() const
    { return m_proj; }
    void setProjection(const Projection & val)
    { m_proj = val; }

    const double & getHFOV() const
    { return m_hfov; }
    void setHFOV(const double & val)
    { m_hfov = val; }

    const std::vector<double> & getRadialDistortion() const
    { return m_radialDist; }
    void setRadialDistortion(const std::vector<double> & val)
    {
        DEBUG_ASSERT(val.size() == 4);
        m_radialDist = val; 
    }

    const FDiff2D & getRadialDistortionCenterShift() const
    { return m_centerShift; }
    void setRadialDistortionCenterShift(const FDiff2D & val)
    { m_centerShift = val; }

    FDiff2D getRadialDistortionCenter() const
    { return FDiff2D(m_size)/2.0 + m_centerShift; }

    const FDiff2D & getShear() const
    { return m_shear; }
    void setShear(const FDiff2D & val)
    { m_shear = val; }

    int getVigCorrMode() const
    { return m_vigCorrMode; }
    void setVigCorrMode(const int & val)
    { m_vigCorrMode = val; }

    const std::string & getFlatfieldFilename() const
    { return m_flatfield; }
    void setFlatfieldFilename(const std::string & val)
    { m_flatfield = val; }

    const std::vector<double> & getRadialVigCorrCoeff() const
    { return m_radialVigCorrCoeff; }
    void setRadialVigCorrCoeff(const std::vector<double> & val)
    { 
        DEBUG_ASSERT(val.size() == 4);
        m_radialVigCorrCoeff = val; 
    }

    const FDiff2D & getRadialVigCorrCenterShift() const
    { return m_radialVigCorrCenterShift; }
    void setRadialVigCorrCenterShift(const FDiff2D & val)
    { m_radialVigCorrCenterShift = val; }

    FDiff2D getRadialVigCorrCenter() const
    { return FDiff2D(m_size)/2.0 + m_radialVigCorrCenterShift; }

    int getLensNr() const
    { return m_lensNr; }
    void setLensNr(const int & val)
    { m_lensNr = val; }

    CropMode getCropMode() const
    { return m_crop; }
    void setCropMode(CropMode val)
    {
        m_crop = val;
        if (m_crop == NO_CROP) {
            m_cropRect = vigra::Rect2D(m_size);
        }
    }

    const vigra::Rect2D & getCropRect() const
    { return m_cropRect; }
    void setCropRect(const vigra::Rect2D & val)
    { m_cropRect = val; }

    const double & getRoll() const
    { return m_roll; }
    void setRoll(const double & val)
    { m_roll = val; }
    const double & getPitch() const
    { return m_pitch; }
    void setPitch(const double & val)
    { m_pitch = val; }
    const double & getYaw() const
    { return m_yaw; }
    void setYaw(const double & val)
    { m_yaw = val; }
    const std::vector<double> & getBrightnessFactor() const
    { return m_ka; }
    void setBrightnessFactor(const std::vector<double> & val)
    { m_ka = val; }
    const std::vector<double> & getBrightnessOffset() const
    { return m_kb; }
    void setBrightnessOffset(const std::vector<double> & val)
    { m_kb = val; }

    const double & getGamma() const
    { return m_gamma; }
    void setGamma(const double & val)
    { m_gamma = val; }

private:
    std::string m_filename;
//    VariableVector m_vars;
    vigra::Size2D m_size;

    Projection m_proj;
    double m_hfov;

    double m_gamma;

    // orientation in degrees
    double m_roll;
    double m_pitch;
    double m_yaw;

    // radial lens distortion
    std::vector<double> m_radialDist;
    // Center shift
    FDiff2D m_centerShift;
    // shear
    FDiff2D m_shear;

    // crop description
    CropMode m_crop;
    vigra::Rect2D m_cropRect;

    int m_vigCorrMode;
    // coefficients for vignetting correction (even degrees: 0,2,4,6, ...)
    std::string m_flatfield;
    std::vector<double> m_radialVigCorrCoeff;
    FDiff2D m_radialVigCorrCenterShift;

    // linear pixel transform
    std::vector<double> m_ka;
    std::vector<double> m_kb;

    unsigned m_lensNr;
    //
    // panotools options
    //
    // u10           specify width of feather for stitching. default:10
    unsigned int m_featherWidth;
    // Morph-to-fit using control points.
    bool m_morph;
};

/** Holds information about the destination image.
 */
class DestPanoImage
{
public:
    enum Projection { RECTILINEAR = 0,
                      CYLINDRICAL = 1,
                      EQUIRECTANGULAR = 2,
                      FULL_FRAME_FISHEYE = 3};
 
    DestPanoImage()
    {
        m_proj = EQUIRECTANGULAR;
        m_hfov = 360;
        m_size = vigra::Size2D(360,180);
        m_roi = vigra::Rect2D(m_size);
    }

    DestPanoImage(Projection proj, double hfov, vigra::Size2D sz)
     : m_proj(proj), m_hfov(hfov), m_size(sz), m_roi(sz)
    {
    }
    bool horizontalWarpNeeded()
    {
        switch (m_proj)
        {
            case CYLINDRICAL:
            case EQUIRECTANGULAR:
                if (m_hfov == 360) return true;
            case FULL_FRAME_FISHEYE:
            case RECTILINEAR:
            default:
                break;
        }
        return false;
    }
    // data accessors
    const Projection & getProjection() const
    { return m_proj; }
    void setProjection(const Projection & val)
    { m_proj = val; }

    const double & getHFOV() const
    { return m_hfov; }
    void setHFOV(const double & val)
    { m_hfov = val; }

    const vigra::Size2D & getSize() const
    { return m_size; }
    void setSize(const vigra::Size2D & val)
    { m_size = val; }

    const vigra::Rect2D & getROI() const
    { return m_roi; }
    void setROI(const vigra::Rect2D & val)
    { m_roi = val; }

private:
    Projection m_proj;
    double m_hfov;
    vigra::Size2D m_size;
    vigra::Rect2D m_roi;
};

    /** This class holds an source image.
     *
     *  It contains information about its settings for the panorama.
     *
     *  An image should not depend on the panorama.
     */
    class PanoImage
    {
    public:
        PanoImage()
        {
            width=0;
            height=0;
            lensNr=0;
        };

        PanoImage(const std::string &filename,  int width,int height,
                  int lens);

//        PanoImage(const std::string & filename);
        // create from xml node
//        PanoImage(QDomNode & node);

        virtual ~PanoImage();

        virtual const char * isA() const { return "PanoImage"; };

        std::string getFilename() const
            { return filename; }

        void setFilename(std::string fn)
            { filename = fn; }

        const ImageOptions & getOptions() const
            { return options; }
    
        void setOptions(const ImageOptions & opt)
            { options = opt; }

        unsigned int getHeight() const
            { return height; }
        unsigned int getWidth() const
            { return width; }

        void setLensNr(unsigned int l)
            { lensNr = l; }
        unsigned int getLensNr() const
            { return lensNr; }

        void setSize(const vigra::Size2D & sz)
            { width =sz.x; height = sz.y; }

        void setFeatherWidth(unsigned int w)
            { options.featherWidth = w; }

    private:
        /// common init for all constructors
        void init();
        /// read image info (size, exif header)
        bool readImageInformation();

        // image properties needed by Panorama tools.

        std::string filename;
        int height,width;

        bool imageRead;
        ImageOptions options;
        // the lens of this image
        unsigned int lensNr;
    };

} // namespace

#endif // PANOIMAGE_H
