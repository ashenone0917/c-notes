```cpp
SECURITY_DESCRIPTOR* getFileSecurityDescriptor(const wchar_t* path) {
    SECURITY_DESCRIPTOR* sd = NULL;
    DWORD len = 0;
    SECURITY_INFORMATION info =
        OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION;

    if (FALSE == GetFileSecurity(path, info, 0, 0, &len)) {
        if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return NULL;
        }
        sd = (SECURITY_DESCRIPTOR*)malloc(len);
    }
    
   
    if (sd == NULL) {
        return NULL;
    }
    else {
        if (!(*GetFileSecurity)(path, info, sd, len, &len)) {
            free(sd);
            sd = NULL;
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

//获取文件拥有者
SID* getFileOwner(SECURITY_DESCRIPTOR* sd) {
    SID* owner;
    BOOL defaulted;

    if (!GetSecurityDescriptorOwner(sd, (PSID*)&owner, &defaulted)) {
        return NULL;
    }
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
        //sid用于测试
        if (EqualSid(owner, sid)) {
            if (access->Mask & FILE_GENERIC_READ || access->Mask & FILE_GENERIC_WRITE) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

BOOL isFileAccessWrite(const wchar_t* path) {
    auto sd = getFileSecurityDescriptor(path);
    if (sd == NULL) return TRUE;

    auto ower = getFileOwner(sd);
    if (ower == NULL) return TRUE;

    ACL* acl = getFileDACL(sd);
    if (acl == NULL) return TRUE;

    if (sd != NULL) ::free(sd);

    return isAccessReadWrite(ower, acl);
}
```
