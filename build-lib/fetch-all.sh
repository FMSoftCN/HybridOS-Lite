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

# hicairo, hirsvg library
for comp in hicairo hirsvg; do
    git clone $LIB_REPO_URL/$comp.git -b $RENDER_BRANCH_NAME
done

# hibox, hidomlayout library
for comp in hibox hidomlayout; do
    git clone $LIB_REPO_URL/$comp.git
done

# hibus
git clone $LIB_REPO_URL/hibus.git -b $HIBUS_BRANCH_NAME

