#!/bin/sh

echo "Abandoned in favor of an action in pr-desktop-tools ;-)"
exit 1

printf "Class Name: "
read className
echo "className: ${className}"
touch "src/${className}.H"