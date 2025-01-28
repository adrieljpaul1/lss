#!bin/bash/

echo "adding files..."
git add .
echo "commiting to"  $PWD
echo "enter commit message!"
read message
git commit -m "$message"
echo "pushing to main at" $PWD
git push origin main
echo "all done ^_^"

