#!/usr/bin/env bash
###
 # @Description: 
 # @version: 1.0.0
 # @Author: qianchengjun
 # @Date: 2022-03-25 11:23:33
 # @LastEditors: qianchengjun
 # @LastEditTime: 2022-03-25 14:00:13
### 

CheckFoldBuild()
{
   # 判断文件夹是否存在
   foldname="build"
   flag="false"
   curFold=$(pwd)
   echo -e "\e[1;36mcmake_QCJ: \e[0m"
   foldpath="$curFold/$foldname"
   # echo $foldpath
   if [ ! -d "$foldpath" ];then
      echo "make directory \"build\"..."
      mkdir $foldpath
      if [ $? -eq 0 ]; then
         flag="true"
      else
         echo -e "\e[1;31m make directory \"build\" failed.\e[0m"
         exit -1
      fi
   fi
}

GetExeFile()
{
   IFS=:
   for folder in $curFold  #PATH变量分隔符为：
   do
      echo $folder
      for file in  $folder/*   # /*表示需要遍历$folder内所有的目录   
      do
         if [[ $file =~ "CMake" ]];then
               continue
         fi
         if [ -x $file ];then
               exePath=$file
               exeFile="${exePath##*/}"
         fi
      done
   done
}

main()
{
   curFold=$(pwd)
   echo "current folder: $curFold"
   curFoldname="${curFold##*/}" # curFold also changed
   if [ "$curFoldname" = "$foldname" ]; then 
      # echo $curFold
      rm -rf *
      cmake ..
      if [ $? -eq 0 ]; then
         flag="true"
         echo -e "\e[1;36mcmake success.\e[0m\n"
      else 
         flag="false"
         echo -e "\e[1;31m cmake failed.\e[0m"
         exit -1
      fi

      if [ "true" == "$flag" ];then
         make
      fi

      if [ $? -eq 0 ]; then
         echo -e "\e[1;36mmake success.\e[0m\n"
         flag="true"
      else 
         flag="false"
         echo -e "\e[1;31m make failed.\e[0m"
         exit -1
      fi
      GetExeFile
      echo -e "\e[1;36mexecute file: $exeFile\e[0m"
      ./$exeFile
   fi
}



foldpath=""
exeFile=""
CheckFoldBuild
cd $foldpath
main