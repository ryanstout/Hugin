// -*- c-basic-offset: 4 -*-
/** @file wxPanoCommand.h
 *
 *  wxwindows specific panorama commands
 *
 *  @author Pablo d'Angelo <pablo.dangelo@web.de>
 *
 *  $Id$
 *
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public
 *  License along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef _WXPANOCOMMAND__H
#define _WXPANOCOMMAND__H

#include "PT/PanoCommand.h"
#include "common/stl_utils.h"
#include "hugin/ImageCache.h"

namespace PT {

/** add image(s) to a panorama */
class wxAddImagesCmd : public PanoCommand
{
public:
    wxAddImagesCmd(Panorama & pano, const std::vector<std::string> & files)
    : PanoCommand(pano), files(files)
    { };
    virtual void execute()
        {
            PanoCommand::execute();

            // FIXME make it possible to add other lenses,
            // for example if the exif data or image size
            // suggests it.
            std::vector<std::string>::const_iterator it;
            for (it = files.begin(); it != files.end(); ++it) {
                const std::string &filename = *it;
                std::string::size_type idx = filename.rfind('.');
                if (idx == std::string::npos) {
                    DEBUG_DEBUG("could not find extension in filename");
                }
                std::string ext = filename.substr( idx+1 );

                wxImage * image = ImageCache::getInstance().getImage(filename);
                int width = image->GetWidth();
                int height = image->GetHeight();

                Lens lens;
                lens.isLandscape = (width > height);
                if (lens.isLandscape) {
                    lens.setRatio(((double)width)/height);
                } else {
                    lens.setRatio(((double)height)/width);
                }

                if (utils::tolower(ext) == "jpg") {
                    // try to read exif data from jpeg files.
                    lens.readEXIF(filename);
                }

                bool addNew = (pano.getNrOfLenses() == 0);
#if 0
                // FIXME: check if the exif information
                // indicates other camera parameters
                for (unsigned int lnr=0; lnr < pano.getNrOfLenses(); lnr++) {
                    const Lens & l = pano.getLens(lnr);
                    // compare lenses.
                    // FIXME check if we need to add another lens
                    // query user if we need to.
                    
                }
#endif
                    
                int lensNr=0;
                if (addNew) {
                    lensNr = pano.addLens(lens);
                }


                VariableMap vars;
                fillVariableMap(vars);

                PanoImage img(filename, width, height, lensNr);
                pano.addImage(img, vars);
            }
            pano.changeFinished();
        }
    virtual std::string getName() const
        {
            return "add images";
        }
private:
    std::vector<std::string> files;
};

} // namespace PT

#endif // _WXPANOCOMMAND__H
