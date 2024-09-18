# Invoke these tests at '\ThinBridge\script' 

Describe "Check if the current version is proper" {
    Context "Compare the extracted current version to the expected" {
        BeforeEach {
            $Env:ThinBridgeVersionUpRootPath = "./fixtures/"
        }
        It "Validity of the current version" {
            # Be sure to update this number according to the version it should be
            $result = .\ThinBridgeVersionUp.ps1
            $result | Should Match "9.8.7.6"
        }
        AfterEach {
            $Env:ThinBridgeVersionUpRootPath = ''
        }
    }
}

Describe "Target files to be existent" {
    Context "For .rc file" {
        It "Exist ThinBridge.rc" {
            "../ThinBridge/ThinBridge.rc" | Should Exist
        } 
        It "Exist BHORedirector.rc" {
            "../BHORedirector/BHORedirector.rc" | Should Exist
        } 
        It "Exist TBo365URLSync.rc" {
            "../TBo365URLSync/TBo365URLSync.rc" | Should Exist
        } 
        It "Exist TBRedirector.rc" {
            "../TBRedirector/TBRedirector.rc" | Should Exist
        } 
        It "Exist ThinBridgeChecker.rc" {
            "../ThinBridgeChecker/ThinBridgeChecker.rc" | Should Exist
        } 
        It "Exist ThinBridgeRuleUpdater.rc" {
            "../ThinBridgeRuleUpdater/ThinBridgeRuleUpdater.rc" | Should Exist
        } 
        It "Exist ThinBridgeX64.iss" {
            "../ThinBridgeX64.iss" | Should Exist
        } 
        It "Exist ThinBridgeX86.iss" {
            "../ThinBridgeX86.iss" | Should Exist
        } 
    }
}

Describe "Other ThinBridgeVersionUp tests" {
    Context "Parameters to fail would include " {
        It "One that consists of more than 2 digits" {
            {.\ThinBridgeVersionUp.ps1 -new_version 0.0.100} | Should Throw
        } 
        It "Negative number" {
            {.\ThinBridgeVersionUp -new_version -1.0.0} | Should Throw
        } 
        It "Less than 4 numbers in total" {
            {.\ThinBridgeVersionUp -new_version 9.8.7} | Should Throw
        } 
        It "Any invalid separator other than period" {
            {.\ThinBridgeVersionUp -new_version 1,2,3} | Should Throw
        } 
    }
}
