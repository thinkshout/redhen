<?php

/**
 * @file
 * 
 */

/**
 * Wrapper for system_admin_menu_block_page().
 */
function redhen_admin_menu_block_page() {
  module_load_include('inc', 'system', 'system.admin');
  return system_admin_menu_block_page();
}

/**
 * Settings form for Redhen CRM.
 *
 * @return array
 */
function redhen_settings() {
  $form['intro'] = array(
     '#type' => 'item',
     '#markup' => t('Redhen CRM settings.'),
   );

  // allow other modules to inject their own settings.
  $form += module_invoke_all('redhen_settings');
  return system_settings_form($form);
}
