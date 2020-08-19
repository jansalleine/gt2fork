#/bin/sh
RUNNER=`whoami`
if [ $RUNNER = "root" ]
then
    APPPATH="/usr/local/share/applications"
    ICONPATH="/usr/local/share/icons"
    INSTALLPATH="/usr/local/bin"
else
    APPPATH="$HOME/.local/share/applications"
    ICONPATH="$HOME/.local/share/icons"
    INSTALLPATH="$HOME/.local/bin"
    mkdir -p "$INSTALLPATH"
fi
rm -f -v "$APPPATH/gt2fork.desktop"
rm -f -v "$INSTALLPATH/gt2fork"
rm -f -v "$ICONPATH/gt2fork.png"
