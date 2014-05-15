<?php

/**
 * @file
 * API documentation for Redhen Dontations module.
 */

/**
 * Override redhen_donation_access with custom access control logic.
 *
 * @param $op
 * @param RedhenDonation $donation
 * @param object $account
 *
 * @return bool
 */
function hook_redhen_donation_access($op, $donation, $account = NULL) {
  if ($donation->user_uid == $account->uid) {
    return TRUE;
  }
}


/**
 * Provide a form API element exposed as a Donation entity setting.
 *
 * @param array $settings
 *   Existing settings values.
 *
 * @return array
 *   A FAPI array for a donation setting.
 */
function hook_redhen_donation_entity_settings($settings) {
  return array(
    'redhen_donation_entity_access_roles' => array(
      '#type' => 'checkboxes',
      '#title' => t('Roles'),
      '#description' => t('Override default access control settings by selecting which roles can donate.'),
      '#options' => user_roles(),
      '#default_value' => isset($settings['settings']['redhen_donation_entity_access_roles']) ? $settings['settings']['redhen_donation_entity_access_roles'] : NULL,
    ),
  );
}

/**
 * Provides a way to alter access to the donation status.
 *
 * @param string $status
 *   The current status
 * @param array $context
 *   Contextual information about the item being altered:
 *   - 'entity_type': The host entity type.
 *   - 'entity_id': The host entity ID.
 *   - 'errors'(optional) An array of error message strings.
 */
function hook_redhen_donation_status_alter($status, $context) {

}
