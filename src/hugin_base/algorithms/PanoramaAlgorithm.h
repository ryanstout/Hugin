// -*- c-basic-offset: 4 -*-
/** @file PanoramaAlgorithm.h
*
*  @author Ippei UKAI <ippei_ukai@mac.com>
*
*  $Id: $
*
*  This is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public
*  License as published by the Free Software Foundation; either
*  version 2 of the License, or (at your option) any later version.
*
*  This software is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this software; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1307, USA.
*
*  Hereby the author, Ippei UKAI, grant the license of this particular file to
*  be relaxed to the GNU Lesser General Public License as published by the Free
*  Software Foundation; either version 2 of the License, or (at your option)
*  any later version. Please note however that when the file is linked to or
*  compiled with other files in this library, the GNU General Public License as
*  mentioned above is likely to restrict the terms of use further.
*
*/

#include <exception>

class PanoramaData;



namespace HuginBase {

    
///        
class MissingRequiredArgumentException : std::exception
{
public:
    
    MissingRequiredArgumentException();
    
    virtual ~MissingRequiredArgumentException();
    
    
    char* what()
        { return "Missing one of required arguments."; };
    
};




/**
 
 */
template <class ReturnType>
class PanoramaAlgorithm
{

public:
    
    virtual PanoramaAlgorithm(PanoramaData& panorama)
        : m_panorama(panorama)
        { };
    
    virtual ~PanoramaAlgorithm();
    
    ///
    virtual bool modifiesPanoramaData();
    
    ///
    virtual ReturnType call() throw (MissingRequiredArgumentException);
    
    // === [TODO: callback and progress reporting etc.] ===
    
    
    /*
     * Here is the informal interface guidelines that you should follow when
     * you subclass from this class:
     *
     *  1. You should have [ void setSomeParameter(some parameter) ] methods
     *   for all parameters of the algorithms if any.
     *
     *  2. You should provide [ ReturnType call(all parameters) ] method for
     *   convenience.
     *
     *  3. You can provide [ SomeType getSomeResult() ] if there are more
     *   results.
     *
     *  4. You can optionaly provide [ static ReturnType executeMyAlgorithm(PanoramaData& panorama, all parameters) ]
     *   as well.
     *
     */

protected:
    
    ///
    PanoramaData& getPanorama() { return m_panorama; };
    
private:
        
    PanoramaData& m_panorama;
    
};


} // namespace

