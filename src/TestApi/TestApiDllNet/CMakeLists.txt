
add_library(
    TestApiDllNet
    SHARED
    Properties/AssemblyInfo.cs
    TestApi.cs
    )

set_property(TARGET TestApiDllNet PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.5")

set_property(TARGET TestApiDllNet PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
    "System.Data"
    "System.Data.DataSetExtensions"
    "System.Xml"
    "System.Xml.Linq"
    )
