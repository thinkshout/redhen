<?php

/**
 * @file
 * 
 */

/**
 * Wrapper for system_admin_menu_block_page().
 */
function redhen_landing_page() {
  $items = array(
    'contacts' => array(
      'path' => 'redhen/contact',
      'title' => 'Contacts',
      'description' => t('View, manage, and search contacts.')
    ),
    'orgs' => array(
      'path' => 'redhen/org',
      'title' => 'Organizations',
      'description' => t('View, manage, and search organizations.')
    ),
    'memberships' => array(
      'path' => 'redhen/memberships',
      'title' => 'Memberships',
      'description' => t('View, manage, and search memberships.')
    ),
    'structure' => array(
      'path' => 'admin/structure/redhen',
      'title' => 'Structure',
      'description' => t('Define bundles and manage fields for RedHen entities.')
    ),
    'settings' => array(
      'path' => 'admin/config/redhen',
      'title' => 'Settings',
      'description' => t('RedHen configuration and settings.')
    ),
  );
  return theme('redhen_home', array('items' => $items));
}

/**
 * Settings form for Redhen CRM.
 *
 * @return array
 */
function redhen_settings() {
  $form['intro'] = array(
     '#type' => 'item',
     '#markup' => t('RedHen CRM settings.'),
   );

  // allow other modules to inject their own settings.
  $form += module_invoke_all('redhen_settings');
  return system_settings_form($form);
}
