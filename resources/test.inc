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
          
                 'index'=> [
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
                             'name' => 'name',
                             'type' => 'string',
                             'description' => 'Type of string kurwa',
                             'source' => array('path'=> '0'),
                             'optional' => TRUE
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
function _timian_service_fetch_data($name){

    return 'Kurwa jest : ' . $name;
}