#!/bin/bash

echo "./do true"
./do true
echo "> renvoie $? (attendu : 0)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo "./do false"
./do false
echo "> renvoie $? (attendu : 1)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo "./do --or true false"
./do --or true false
echo "> renvoie $? (attendu : 1)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo "./do --and true false"
./do --and true false
echo "> renvoie $? (attendu : 0)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo './do --or "xclock" "xclock -d" "xclock -d -brief"'
echo "> /!\ (3 xclock différents apparaissent => les options sont supportées)"
./do --or "xclock" "xclock -d" "xclock -d -brief"
echo "> renvoie $? (attendu : 0)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo './do --or --cc true xclock'
./do --or --cc true xclock
echo "> renvoie $? (attendu : 0)"
echo "> /!\ (ce message apparait et une xclock tourne toujours => cc supporté)"

echo "appuyez sur Entrée pour passer à la suite (tuera la clock restante)"
read
killall -q xclock

echo './do --or --cc --kill true xclock'
./do --or --cc --kill true xclock
echo "> renvoie $? (attendu : 0)"
echo "> /!\ (ce message apparait et la xclock a été kill => kill supporté)"

echo "appuyez sur Entrée pour passer à la suite"
read

echo "Merci de votre attention !"

