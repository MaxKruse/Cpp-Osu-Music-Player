msbuild /property:Configuration=Dist

cd ./bin/Dist-windows-x86_64/Player
rem Player.pdb

copy ./../../../Player/vendor/Un4Seen_bass/bass.dll bass.dll
copy ./../../../Player/vendor/Un4Seen_bass/bass_fx.dll bass_fx.dll

7z a Release.zip bass.dll bass_fx.dll Parser.dll Player.exe