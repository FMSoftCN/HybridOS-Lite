# This is the template of `myconfig.sh`, and the later
# will be included in every script.

# PLEASE COPY THIS FILE TO `myconfig.sh`,
# AND CHANGE THE FOLLOWING GLOBAL VARIABLES FOR YOUR CUSTOMIZATION.

#
# The URL prefix of remote repository.

# Use this if you want to visit GitHub via HTTPS
GITHUB_REPO_URL=https://github.com/VincentWei

#
# Use this one if you are a developer of MiniGUI
MINIGUI_REPO_URL=https://gitlab.fmsoft.cn/VincentWei/

#
# Use this one to get library necessary
LIB_REPO_URL=https://gitlab.fmsoft.cn/hybridos/


# The branch name for MINIGUI
MINIGUI_BRANCH_NAME=rel-5-0

#
# The branch name for hicairo and hirsvg
RENDER_BRANCH_NAME=minigui-backend

#
# The branch name for hibus
HIBUS_BRANCH_NAME=rel-1-0

#
# The branch name for hidomlayout
HIDOMLAYOUT_BRANCH_NAME=dev

#
# configuration options for MiniGUI Core.
# GOPTS="--enable-develmode"
MINIGUI_OPTS="--enable-procs --enable-compositing --enable-virtualwindow --disable-Werror --disable-static --enable-videodrm --enable-pixman"

#
# configuration options for mgeff
MGEFF_OPTS="--disable-effectors"

#
# configuration options for hibus
HIBUS_OPTS="-DBUILD_APP_AUTH=OFF"

#
# The jobs number for building source.
NRJOBS=`getconf _NPROCESSORS_ONLN`

