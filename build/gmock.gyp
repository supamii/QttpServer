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
                '../lib/gmock/include',
                '../lib/gmock'
            ],
            'sources' : [
                '../lib/gmock/src/gmock-all.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../lib/gmock/include'
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
                '../lib/gmock/src/gmock_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../lib/gmock/gtest/include',
                    '../lib/gmock/include'
                ]
            }
        }
    ]
}
