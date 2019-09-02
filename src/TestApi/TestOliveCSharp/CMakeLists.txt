
add_executable(
    TestOliveCSharp
    App.config
    Form1.cs
    Form1.Designer.cs
    Form1.resx
    Program.cs
    TestOlive.cs
    Properties/AssemblyInfo.cs
    Properties/Resources.Designer.cs
    Properties/Resources.resx
    Properties/Settings.Designer.cs
    Properties/Settings.settings
)
target_link_libraries(TestOliveCSharp PRIVATE TestOliveCppCli)

set_property(TARGET TestOliveCSharp PROPERTY VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.5")

set_property(TARGET TestOliveCSharp PROPERTY VS_DOTNET_REFERENCES
    "Microsoft.CSharp"
    "System"
    "System.Core"
    "System.Deployment"
    "System.Data"
    "System.Data.DataSetExtensions"
    "System.Drawing"
    "System.Xml"
    "System.Xml.Linq"
    "System.Windows.Forms"
    )
