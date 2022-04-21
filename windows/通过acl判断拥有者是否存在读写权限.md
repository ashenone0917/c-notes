```cpp
SECURITY_DESCRIPTOR* getFileSecurityDescriptor(const char* path) {
    SECURITY_DESCRIPTOR* sd = NULL;
    DWORD len = 0;
    SECURITY_INFORMATION info =
        OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;

    if (FALSE == GetFileSecurityA(path, info, 0, 0, &len)) {
        if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return NULL;
        }
        sd = (SECURITY_DESCRIPTOR*)malloc(len);
    }
    
   
    if (sd == NULL) {
        return NULL;
    }
    else {
        if (!(*GetFileSecurityA)(path, info, sd, len, &len)) {
            free(sd);
            return NULL;
        }
    }

    return sd;
}

ACL* getFileDACL(SECURITY_DESCRIPTOR* sd) {
    ACL* acl;
    int defaulted, present;

    if (!GetSecurityDescriptorDacl(sd, &present, &acl, &defaulted)) {
        return NULL;
    }

    if (!present) {
        return NULL;
    }

    return acl;
}

BOOLEAN GetInfo(PSID sid, LPWSTR* ppName, LPWSTR* ppDomain)
{
    DWORD dwNameSize = 0;
    DWORD dwDomainNameSize = 0;
    SID_NAME_USE snu;
    if (!LookupAccountSid(NULL, sid, NULL, &dwNameSize, NULL, &dwDomainNameSize, &snu)) {
        if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
            return FALSE;
        }
    }
    *ppName = (LPWSTR)malloc(dwNameSize);
    if (NULL == ppName) {
        return FALSE;
    }
    *ppDomain = (LPWSTR)malloc(dwDomainNameSize);
    if (NULL == ppName) {
        DWORD err = GetLastError();
        free(*ppName);
        SetLastError(err);
        return FALSE;
    }
    if (!LookupAccountSid(NULL, sid, *ppName, &dwNameSize, *ppDomain, &dwDomainNameSize, &snu)) {
        DWORD err = GetLastError();
        free(*ppName);
        free(*ppDomain);
        SetLastError(err);
        return FALSE;
    }

    return TRUE;
}

//获取文件拥有者
SID* getFileOwner(SECURITY_DESCRIPTOR* sd) {
    SID* owner;
    BOOL defaulted;

    if (!GetSecurityDescriptorOwner(sd, (PSID*)&owner, &defaulted)) {
        return NULL;
    }
    //LPWSTR temp = NULL;
    //ConvertSidToStringSid(owner, &temp);
    //LPWSTR temp1 = NULL;
    //LPWSTR temp2 = NULL;
    //GetInfo(owner, &temp1, &temp2);
    return owner;
}
#define RW_ACCESS (FILE_GENERIC_READ | FILE_GENERIC_WRITE)

BOOL isAccessReadWrite(SID* owner, ACL* acl) {
    ACL_SIZE_INFORMATION acl_size_info{};
    DWORD i;

    if (acl == NULL) {
        return TRUE;
    }

    if (!GetAclInformation(acl, (void*)&acl_size_info, sizeof(acl_size_info),
        AclSizeInformation)) {
        return FALSE;
    }

    for (i = 0; i < acl_size_info.AceCount; i++) {
        void* ace;
        ACCESS_ALLOWED_ACE* access;
        SID* sid;

        if (!GetAce(acl, i, &ace)) {
            return FALSE;
        }
        if (((ACCESS_ALLOWED_ACE*)ace)->Header.AceType != ACCESS_DENIED_ACE_TYPE) {
            continue;
        }
        access = (ACCESS_ALLOWED_ACE*)ace;
        sid = (SID*)&access->SidStart;
        if (EqualSid(owner, sid)) {
            if (access->Mask & FILE_GENERIC_READ || access->Mask & FILE_GENERIC_WRITE) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL isFileAccessWrite(const char* path) {
    auto sd = getFileSecurityDescriptor(path);
    if (sd == NULL) return TRUE;

    auto ower = getFileOwner(sd);
    if (ower == NULL) return TRUE;

    ACL* acl = getFileDACL(sd);
    if (acl == NULL) return TRUE;

    return isAccessReadWrite(ower, acl);
}
```
