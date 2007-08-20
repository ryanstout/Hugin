// -*- c-basic-offset: 4 -*-
/** @file utils.h
 *
 *  @author Pablo d'Angelo <pablo.dangelo@web.de>
 *
 *  $Id: utils.h 1814 2006-12-31 14:37:05Z dangelo $
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

#ifndef _Hgn1_PANORAMA_utils_H
#define _Hgn1_PANORAMA_utils_H

#include <algorithms/panorama_makefile/PanoramaMakefileExport.h>

#include "PT/Panorama.h"


namespace PT {


struct PTPrograms : public HuginBase::PanoramaMakefileExport::PTPrograms {};


/** create a makefile and associated project file for rendering */
void createMakefile(const Panorama & pano,
                    const UIntSet & images,
                    const std::string & ptofile,
                    const std::string & outputPrefix,
                    const PTPrograms & progs,
                    const std::string & includePath,
                    std::ostream & o)
{   
    Panorama copyOfPano(pano);
    HuginBase::PanoramaMakefileExport(copyOfPano, o, images, ptofile, outputPrefix, progs, includePath).run();
};


} // namespace




#endif // _PANORAMA_utils_H
