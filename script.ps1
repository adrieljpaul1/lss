Write-host "adding files..."
git add .
Write-Host "Add success! refer above for messages from git!"
Write-host "enter your COMMIT MESSAGE!"
$message = Read-host
Write-host "committing..."
git commit -m "$message"
Write-host "Pushing to $PWD"
git push origin main
Write-host "sucess ^_^"
