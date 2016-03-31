<?php

/**
 * @file
 * Describes API functions for the RedHen module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Provide a form API element exposed as a RedHen setting.
 *
 * @param \Drupal\Core\Config\Config $config
 *   The redhen.settings config object.
 *
 * @return array
 */
function hook_redhen_settings(Drupal\Core\Config\Config $config) {
  return array(
    'redhen_contact_connect_users' => array(
      '#type' => 'checkbox',
      '#title' => t('Connect users to Redhen contacts'),
      '#description' => t('If checked, Redhen will attempt to connect Drupal users to Redhen contacts by matching email addresses when a contact is updated.'),
      '#default_value' => $config->get('redhen_contact_connect_users'),
    ),
  );
}


/**
 * @} End of "addtogroup hooks".
 */
