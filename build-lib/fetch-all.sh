#!/bin/bash
  
if [ ! -f myconfig.sh ]; then
    cp config.sh myconfig.sh
fi

source myconfig.sh

# GVFB
git clone $GITHUB_REPO_URL/gvfb.git

# MiniGUI, mgeff and resource
for comp in minigui-res minigui mgeff; do
    git clone $MINIGUI_REPO_URL/$comp.git -b $MINIGUI_BRANCH_NAME
done

# hicairo library
for comp in hicairo; do
    git clone $LIB_REPO_URL/$comp.git -b $RENDER_BRANCH_NAME
done

# hibox, hisvg library
for comp in hibox hisvg; do
    git clone $LIB_REPO_URL/$comp.git
done

# hidomlayout
for comp in hidomlayout; do
    git clone $LIB_REPO_URL/$comp.git -b $HIDOMLAYOUT_BRANCH_NAME
done


# hibus
git clone $LIB_REPO_URL/hibus.git -b $HIBUS_BRANCH_NAME

