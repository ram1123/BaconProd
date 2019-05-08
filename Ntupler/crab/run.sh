#!/bin/bash

label=$1
dataset=$2
config=$3
range=$4
rm -rf crab_projects/crab_${label}
rm -rf /eos/uscms/store/user/rasharma/FirstStep/${label}
mkdir /eos/uscms/store/user/rasharma/FirstStep/${label}
#rm -rf /eos/uscms/store/user/cmantill/${label}
#mkdir /eos/uscms/store/user/cmantill/${label}
if [ $config == "makingBacon_Data_25ns_MINIAOD.py" ]; then
    sed "s@XX-LABEL-XX@$label@g" crab_template_data.py | sed "s@XX-DATASET-XX@$dataset@g" | sed "s@XX-CONFIG-XX@$config@g" | sed "s@XX-RANGE-XX@$range@g"  > crab.py
fi
if [ $config == "makingBacon_Data_25ns_MINIAOD_noPF.py" ]; then
    sed "s@XX-LABEL-XX@$label@g" crab_template_data.py | sed "s@XX-DATASET-XX@$dataset@g" | sed "s@XX-CONFIG-XX@$config@g"   > crab.py
fi
if [ $config == "makingBacon_MC_25ns_MINIAOD.py" ]; then
    sed "s@XX-LABEL-XX@$label@g" crab_template_mc.py | sed "s@XX-DATASET-XX@$dataset@g" | sed "s@XX-CONFIG-XX@$config@g"   > crab.py
fi
if [ $config == "makingBacon_Gen_25ns_MINIAOD.py" ]; then
    sed "s@XX-LABEL-XX@$label@g" crab_template_sig.py | sed "s@XX-DATASET-XX@$dataset@g" | sed "s@XX-CONFIG-XX@$config@g"   > crab.py
fi
#crab submit --dryrun crab.py
crab submit -c crab.py
mv crab.py old/crab_${label}.py
