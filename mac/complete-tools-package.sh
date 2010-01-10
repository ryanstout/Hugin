#!/bin/sh

# $Id: complete-tools-package.sh $

dylib_dir="$REPOSITORY_DIR/lib"
old_install_name_dirname="$REPOSITORY_ABSOLUTE_PATH/lib"
dylib_install_loc="Libraries"
new_install_name_dirname="@executable_path/../$dylib_install_loc"

Package="$TARGET_BUILD_DIR/Hugin_tools"

archs="ppc i386 ppc64 x86_64"
libs="libwx_macu-$WX_MAJOR_VERSION libwx_macu_gl-$WX_MAJOR_VERSION libpano13 $BOOST_THREAD_LIB-$BOOST_VER libpng libtiff libjpeg libIex libImath libIlmImf libIlmThread libHalf libexpat liblcms libintl libgettextsrc-$GETTEXT_VERSION libgettextpo libgettextlib-$GETTEXT_VERSION libasprintf" 
#frameworks="HuginBaseWx.framework HuginBase.framework"
bins="align_image_stack autooptimiser calibrate_lens celeste cpclean deghosting_mask fulla matchpoint pano_trafo pto2mk tca_correct vig_optimize"
binaries="$Package/$dylib_install_loc/*.dylib $Package/bin/* $Package/Frameworks/Hugin*.framework/Hugin*"

#------------------------------------------------

rm -Rf $Package
mkdir -p $Package
cp -Rf ../mac/Hugin_tools/* $Package
#rm -Rf "$Package/$dylib_install_loc" "$Package/Frameworks" "$Package/bin" 
mkdir -p "$Package/$dylib_install_loc" "$Package/Frameworks" "$Package/bin"
for bin in $bins
do
 cp $TARGET_BUILD_DIR/$bin "$Package/bin/" 
done
cp -Rf  $TARGET_BUILD_DIR/Hugin*.framework "$Package/Frameworks"

for dylib in $libs
do
 cp -Rf $dylib_dir/$dylib.*ylib "$Package/$dylib_install_loc/"
done


#replace any "$old_install_name_dirname[/*]/" in install_name to "$new_install_name_dirname/" 
for exec_file in $binaries
do
 
 echo "Processing: $exec_file"
 
 if [[ $exec_file = *.dylib ]]
 then
  for lib in $(otool -D $exec_file | grep $old_install_name_dirname | sed -e 's/ (.*$//' -e 's/^.*\///')
  do
   echo " Changing own install name."
   install_name_tool -id "$new_install_name_dirname/$lib" $exec_file
  done
 fi
 
 for lib in $(otool -L $exec_file | grep $old_install_name_dirname | sed -e 's/ (.*$//' -e 's/^.*\///')
 do
  echo " Changing install name for: $lib"
  install_name_tool -change "$old_install_name_dirname/$lib" "$new_install_name_dirname/$lib" $exec_file
 done

done
