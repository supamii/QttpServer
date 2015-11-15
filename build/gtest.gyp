{
    'targets' : [
        #test
        {
            'target_name' : 'gtest',
            'type' : 'static_library',
            'include_dirs' : [
                '../lib/gmock/gtest/include',
                '../lib/gmock/gtest'
            ],
            'sources' : [
                '../lib/gmock/gtest/src/gtest-all.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../lib/gmock/gtest/include'
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
                '../lib/gmock/gtest/src/gtest_main.cc'
            ],
            'direct_dependent_settings' : {
                'include_dirs' : [
                    '../lib/gmock/gtest/include'
                ]
            }
        }
    ]
}
