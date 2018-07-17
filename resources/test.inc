<?php

/**
 * Function to define resources
 * 
 * @return array $config
 *  Array of resource definition
 */

 function _timian_service_resource_definition() {
     $config = [
         'testpoint' => [
             'actions' => [
                 'check'=> [
                     'help' => 'Test point',
                     'access callback' => '_timian_service_data_access',
                     'access arguments' => ['config'],
                     'access arguments append' => TRUE,
                     'callback'=> '_timian_service_fetch_data',
                     'file' =>[
                         'type' => 'inc',
                         'module' => 'timian_service',
                         'name' => 'resources/test'
                     ],
                     'args'=>[
                         [
                             'name' => 'type',
                             'type' => 'string',
                             'description' => 'Type of string kurwa',
                             'source' => ['data' => 'type'],
                             'optional' => TRUE
                         ]
                     ]
                ]
             ]
         ]
                         ];
                         return $config;
 }

 /**
  * Access restriction.
  * @return bool
  *     Returns boolen
  */
  function _timian_service_data_access($op = 'config', $args = []) {
    $permission = FALSE;
    global $user;
    $roles = $user->roles;

    if (in_array('administrator', $roles)){
        $permission = TRUE;
    }
    return $permission; 
}

 /**
  * Callback function
  */
function _timian_service_fetch_data($str = NULL){
    $data = [
        'message' => 'Success'
    ];
    return $data;
}