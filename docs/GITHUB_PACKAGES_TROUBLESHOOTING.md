# GitHub Packages Troubleshooting Guide

## Common Issues and Solutions

### Issue 1: "maven package 'conan.v2' does not exist"

**Error Message:**
```
ERROR: b'maven package "conan.v2" does not exist under owner "makiolo"\n'
Invalid server response, check remote URL and try again. [Remote: github]
```

**Causes:**
- The package hasn't been published yet (GitHub Actions hasn't run)
- The remote URL is incorrect
- Credentials are invalid
- The repository's GitHub Packages settings are not properly configured

**Solutions:**

#### Step 1: Verify GitHub Actions Published the Package

1. Go to: https://github.com/makiolo/design-patterns-cpp14/actions
2. Look for the "Publish Conan Package" workflow
3. Check if it has a green checkmark (successful run)

If you see red X (failed):
- Click the workflow run to see error details
- Check GitHub Actions logs

#### Step 2: Verify GitHub Packages is Enabled

1. Go to: https://github.com/makiolo/design-patterns-cpp14/settings
2. Navigate to "Packages" section
3. Ensure "GitHub Packages" is enabled
4. Confirm repository permissions are correct

#### Step 3: Check Your Token Permissions

Your Personal Access Token (PAT) needs:
- `read:packages` - to read packages
- `write:packages` - to publish packages  
- `repo` - for private repositories (if needed)

**To create a proper token:**
1. Go to: https://github.com/settings/tokens
2. Click "Generate new token"
3. Select scopes:
   - ✅ `read:packages`
   - ✅ `write:packages`
   - ✅ `repo` (if private)
4. Copy the token (you won't see it again!)

#### Step 4: Reconfigure Your Remote

```bash
# Remove old remote
conan remote remove github || true

# Add with correct URL
conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan" \
  -f

# Login with correct credentials
conan remote login github YOUR_GITHUB_USERNAME -p YOUR_TOKEN
```

---

### Issue 2: "401 Unauthorized" When Uploading

**Error Message:**
```
ERROR: Upload recipe 'design-patterns-cpp14/1.0.24' to 'github': 401 Unauthorized
```

**Solutions:**

1. **Check your credentials are correct:**
   ```bash
   conan remote login github YOUR_USERNAME -p YOUR_TOKEN
   ```

2. **Verify the token has write permissions:**
   - Token must include `write:packages` scope
   - Create new token if unsure

3. **Clear cached credentials:**
   ```bash
   rm ~/.conan2/credentials.json
   conan remote login github YOUR_USERNAME -p YOUR_TOKEN
   ```

---

### Issue 3: "Package Not Found" When Installing

**Error Message:**
```
ERROR: Recipe not found: 'design-patterns-cpp14/1.0.24'
```

**Solutions:**

1. **Check the package was published:**
   - Go to: https://github.com/makiolo/design-patterns-cpp14/packages
   - Look for `design-patterns-cpp14` package
   - If not there, GitHub Actions may not have run

2. **Verify version number matches:**
   ```bash
   # Check what version is available
   conan search design-patterns-cpp14 --remote=github
   ```

3. **Ensure remote is added:**
   ```bash
   conan remote list
   ```
   Should show `github` in the list

4. **Try refreshing the remote:**
   ```bash
   conan remote update github
   ```

---

### Issue 4: "Authentication Failure" During conan install

**Error Message:**
```
ERROR: WARN: Remotes credentials not found, using 'conans' user
```

**Solutions:**

1. **Make sure you're logged in:**
   ```bash
   conan remote login github YOUR_USERNAME -p YOUR_TOKEN
   ```

2. **Verify credentials file exists:**
   ```bash
   cat ~/.conan2/credentials.json
   ```
   Should contain your GitHub credentials

3. **Logout and login again:**
   ```bash
   conan remote logout github
   conan remote login github YOUR_USERNAME -p YOUR_TOKEN
   ```

---

### Issue 5: "Certificate Verify Failed" (SSL Error)

**Error Message:**
```
ERROR: SSL: CERTIFICATE_VERIFY_FAILED
```

**Solutions:**

**NOT RECOMMENDED** - Only as temporary workaround:
```bash
# This is insecure! Only for testing
conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan" \
  -f --insecure
```

**RECOMMENDED** - Fix properly:
1. Update CA certificates:
   ```bash
   # On Ubuntu/Debian
   sudo apt-get install ca-certificates
   
   # On macOS
   brew install openssl
   ```

2. Point Conan to certificates:
   ```bash
   export CONAN_CACERT=/etc/ssl/certs/ca-certificates.crt
   ```

---

## Debug Commands

### List remotes
```bash
conan remote list
```

### Test connection to remote
```bash
conan remote list-pkg design-patterns-cpp14 --remote=github
```

### Clear cache and retry
```bash
conan cache clean design-patterns-cpp14 --confirm
conan install --requires="design-patterns-cpp14/1.0.24" --remote=github
```

### Check Conan configuration
```bash
conan config show remotes
conan config show credentials
```

### View detailed debug output
```bash
conan install --requires="design-patterns-cpp14/1.0.24" \
  --remote=github \
  -v
```

---

## Step-by-Step: Fresh Setup

If everything is broken, try fresh setup:

```bash
# 1. Remove old Conan data
rm -rf ~/.conan2

# 2. Create fresh Conan home
mkdir -p ~/.conan2

# 3. Add remote
conan remote add github \
  "https://maven.pkg.github.com/makiolo/design-patterns-cpp14/conan"

# 4. Login with valid token
conan remote login github YOUR_USERNAME -p YOUR_TOKEN

# 5. Test connection
conan search design-patterns-cpp14 --remote=github

# 6. Try install
conan install --requires="design-patterns-cpp14/1.0.24" --remote=github
```

---

## Getting Help

If issues persist:

1. **Check GitHub Actions logs:**
   - https://github.com/makiolo/design-patterns-cpp14/actions
   - Click failed workflow for detailed error messages

2. **Check GitHub Packages settings:**
   - https://github.com/makiolo/design-patterns-cpp14/settings/packages

3. **Verify your token:**
   - https://github.com/settings/tokens
   - Check expiration date and permissions

4. **Report an issue:**
   - https://github.com/makiolo/design-patterns-cpp14/issues

---

## Quick Reference: URLs

| What | URL |
|---|---|
| GitHub Actions (publishing) | https://github.com/makiolo/design-patterns-cpp14/actions |
| GitHub Packages (downloads) | https://github.com/makiolo/design-patterns-cpp14/packages |
| Repository Settings | https://github.com/makiolo/design-patterns-cpp14/settings |
| Personal Tokens | https://github.com/settings/tokens |
| Releases Page | https://github.com/makiolo/design-patterns-cpp14/releases |

