# Pill Dispenser

## Instructions
* use the provided .clang-format for formatting
1. Clone the repository
```bash
git clone git@github.com:teotoivo/pill_dispenser.git
```

2. Create new branch
```bash
git checkout -b branch-name
```

3. Add a feature / make changes
* Add files changed
```bash
git add .
```
* Commit changes IMPORTANT USE GOOD COMMIT MESSAGES
```bash
git commit -m "Good commit message"
```
* push changes
```bash
git push origin branch-name
```

4. Before creating a pull request make sure the code is up to date
* switch branch to main
```bash
git checkout main
```
* Pull latest changes
```bash
git pull origin main
```
* switch back to your branch
```bash
git checkout branch-name
```
* merge the changes
```bash
git merge main
```
* the readd and commit and push before making the pull request

5. Open a pull request
Once your branch is pushed to GitHub, follow these steps to create a pull request:
* Go to the GitHub repository.
* You should see a notification suggesting that your branch was pushed. Click the Compare & Pull Request button.
* In the PR description, explain the changes you made and why they’re important.
* Select the base branch as main (we’ll be merging your changes into this branch).
* Click Create Pull Request.

## resolving merge conflicts
* Git will alert if there are merge conflicts go and fix the files with conflicts
* then commit them
```bash
git add .
git commit -m "Resolved merge conflicts"
```

## IMPORTANT
* Pushing to main is disabled do not push to main
