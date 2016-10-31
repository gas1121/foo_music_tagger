# foobar2000
include_external_msproject(pfc ${PROJECT_SOURCE_DIR}/ThirdParty/pfc/pfc.vcxproj)
set_property(TARGET pfc PROPERTY FOLDER "ThirdParty")

include_external_msproject(foobar2000_ATL_helpers ${PROJECT_SOURCE_DIR}/ThirdParty/foobar2000/ATLHelpers/foobar2000_ATL_helpers.vcxproj)
set_property(TARGET foobar2000_ATL_helpers PROPERTY FOLDER "ThirdParty")

include_external_msproject(foobar2000_component_client ${PROJECT_SOURCE_DIR}/ThirdParty/foobar2000/foobar2000_component_client/foobar2000_component_client.vcxproj)
set_property(TARGET foobar2000_component_client PROPERTY FOLDER "ThirdParty")

include_external_msproject(foobar2000_sdk_helpers ${PROJECT_SOURCE_DIR}/ThirdParty/foobar2000/helpers/foobar2000_sdk_helpers.vcxproj)
set_property(TARGET foobar2000_sdk_helpers PROPERTY FOLDER "ThirdParty")

include_external_msproject(foobar2000_SDK ${PROJECT_SOURCE_DIR}/ThirdParty/foobar2000/SDK/foobar2000_SDK.vcxproj)
set_property(TARGET foobar2000_SDK PROPERTY FOLDER "ThirdParty")
