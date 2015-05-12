{
    'targets' : [
        #test
        {
            'target_name' : 'gmock',
            'type' : 'static_library',
            'dependencies': [
                './gtest.gyp:gtest'
            ],
            'include_dirs' : [
                '../deps/gmock/include',
                '../deps/gmock'
            ],
            'sources' : [
                '../deps/gmock/src/gmock-all.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/gmock/include'
                ]
            },
            'export_dependent_settings' : [
                './gtest.gyp:gtest'
            ]
        },
        #main
        {
            'target_name' : 'gmock_main',
            'type' : 'static_library',
            'dependencies': [
                'gmock'
            ],
            'sources' : [
                '../deps/gmock/src/gmock_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/gmock/gtest/include',
                    '../deps/gmock/include'
                ]
            }
        }
    ]
}
