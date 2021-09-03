#/bin/sh
EXE="../linux/gt2fork"
if [ ! -f "$EXE" ]
then
    echo "[ERROR] $EXE does not exist."
    exit 1
fi
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
    echo "running 'make install' as normal user"
    echo "binary will be installed to $INSTALLPATH"
    mkdir -p "$INSTALLPATH"
fi
rm -f "$APPPATH/gt2fork.desktop"
cp -f -v "$EXE" "$INSTALLPATH/"
cp -f -v "res/gt2fork.png" "$ICONPATH/"
echo "touch $APPPATH/gt2fork.desktop"
touch "$APPPATH/gt2fork.desktop"
echo "[Desktop Entry]" >> "$APPPATH/gt2fork.desktop"
echo "Name=gt2fork" >> "$APPPATH/gt2fork.desktop"
echo "Exec=$INSTALLPATH/gt2fork" >> "$APPPATH/gt2fork.desktop"
echo "Type=Application" >> "$APPPATH/gt2fork.desktop"
echo "StartupNotify=false" >> "$APPPATH/gt2fork.desktop"
echo "Icon=$ICONPATH/gt2fork.png" >> "$APPPATH/gt2fork.desktop"
