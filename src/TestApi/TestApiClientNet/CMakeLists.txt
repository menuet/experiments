
add_executable(
    TestApiClientNet
    Properties/AssemblyInfo.cs
    Program.cs
    App.config
    )
target_link_libraries(TestApiClientNet PRIVATE TestApiDllNet)
set_property(TARGET TestApiClientNet PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.5")

set_property(TARGET TestApiClientNet PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
    "System.Data"
    "System.Data.DataSetExtensions"
    "System.Xml"
    "System.Xml.Linq"
    )
