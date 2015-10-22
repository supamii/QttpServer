{
    'targets' : [
        #test
        {
            'target_name' : 'gtest',
            'type' : 'static_library',
            'include_dirs' : [
                '../deps/gmock/gtest/include',
                '../deps/gmock/gtest'
            ],
            'sources' : [
                '../deps/gmock/gtest/src/gtest-all.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/gmock/gtest/include'
                ]
            }
        },
        #main
        {
            'target_name' : 'gtest_main',
            'type' : 'static_library',
            'dependencies': [
                'gtest'
            ],
            'sources' : [
                '../deps/gmock/gtest/src/gtest_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../deps/gmock/gtest/include'
                ]
            }
        }
    ]
}
