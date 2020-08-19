#/bin/sh
EXE="../linux/gt2fork"
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
rm -f "$APPPATH/gt2fork.desktop"
cp -f -v "$EXE" "$INSTALLPATH/"
cp -f -v "gt2fork.png" "$ICONPATH/"
touch "$APPPATH/gt2fork.desktop"
echo "[Desktop Entry]" >> "$APPPATH/gt2fork.desktop"
echo "Name=gt2fork" >> "$APPPATH/gt2fork.desktop"
echo "Exec=$INSTALLPATH/gt2fork" >> "$APPPATH/gt2fork.desktop"
echo "Type=Application" >> "$APPPATH/gt2fork.desktop"
echo "StartupNotify=true" >> "$APPPATH/gt2fork.desktop"
echo "Icon=$ICONPATH/gt2fork.png" >> "$APPPATH/gt2fork.desktop"
