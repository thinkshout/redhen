<?php

/**
 * Return a form for an entity's donation settings.
 *
 * @param array $form
 * @param array $form_state
 * @param object $entity_type
 * @param object $entity
 *
 * @return array $form
 *
 * @see hook_redhen_donation_entity_settings().
 */
function redhen_donation_entity_settings_form($form, &$form_state, $settings, $entity_type = NULL, $entity_id = NULL) {
  if ($entity_id) {
    // We'll need this info when we submit the form:
    $form_state['entity'] = array(
      'entity_id' => $entity_id,
      'entity_type' => $entity_type
    );
  }
  // Check to see if date_popup is installed so we can provide a friendlier UI.
  $date_popup_installed = FALSE;
  if (module_exists('date_popup')) {
    $date_popup_installed = TRUE;
  }
  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Check to enable donations.'),
    '#default_value' => isset($settings['status']) ? $settings['status'] : -1,
  );

  $form['scheduling'] = array(
    '#type' => 'fieldset',
    '#title' => t('Scheduling'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE
  );

  $form['scheduling']['open'] = array(
    '#type' => 'textfield',
    '#title' => t('Open Date'),
    '#maxlength' => 25,
    '#description' => t('When to automatically open donations. Format: %time. The date format is YYYY-MM-DD and %timezone is the time zone offset from UTC. Leave blank to disable.', array(
        '%time' => !empty($settings['open']) ? date_format(date_create($settings['open']), 'Y-m-d H:i:s O') : format_date(time(), 'custom', 'Y-m-d H:i:s O'),
        '%timezone' => !empty($settings['open']) ? date_format(date_create($settings['open']), 'O') : format_date(time(), 'custom', 'O'),
      )
    ),
    '#default_value' => !empty($settings['open']) ? $settings['open'] : '',
  );
  if ($date_popup_installed) {
    $form['scheduling']['open']['#type'] = 'date_popup';
    $form['scheduling']['open']['#format'] = 'Y-m-d H:i:s O';
    $form['scheduling']['open']['#description'] = t('When to automatically open donations. (This uses the !timezone.)', array(
      '!timezone' => l(t('site default time zone'), 'admin/config/regional/settings')));
    unset($form['scheduling']['open']['#maxlength']);
  }

  $form['scheduling']['close'] = array(
    '#type' => 'textfield',
    '#title' => t('Close Date'),
    '#maxlength' => 25,
    '#description' => t('When to automatically close donations. Format: %time. The date format is YYYY-MM-DD and %timezone is the time zone offset from UTC. Leave blank to disable.', array(
        '%time' => !empty($settings['close']) ? date_format(date_create($settings['close']), 'Y-m-d H:i:s O') : format_date(time(), 'custom', 'Y-m-d H:i:s O'),
        '%timezone' => !empty($settings['close']) ? date_format(date_create($settings['close']), 'O') : format_date(time(), 'custom', 'O'),
      )
    ),
    '#default_value' => !empty($settings['close']) ? $settings['close'] : '',
  );
  if ($date_popup_installed) {
    $form['scheduling']['close']['#type'] = 'date_popup';
    $form['scheduling']['close']['#format'] = 'Y-m-d H:i:s O';
    $form['scheduling']['close']['#description'] = t('When to automatically close donations. (This uses the !timezone.)', array(
      '!timezone' => l(t('site default time zone'), 'admin/config/regional/settings')));
    unset($form['scheduling']['close']['#maxlength']);
  }

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );


  $form['settings']['donation_entry'] = array(
    '#type' => 'select',
    '#title' => t('Donation entry'),
    '#default_value' => $settings['settings']['donation_entry'],
    '#options' => array('select' => t('Select list'), 'select_other' => t("Select list (with 'other' option)"), 'custom' => t('Text field (custom amount)')),
  );

  $form['settings']['donation_amounts'] = array(
    '#type' => 'textarea',
    '#title' => t('Donation amounts'),
    '#default_value' => redhen_allowed_amounts_string($settings['settings']['donation_amounts']),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          'value' => 'custom'
        ),
      ),
    ),
    '#description' => t('List of allowed donation amounts. Enter one value per line, in the format amount|label. The amount should be entered as an integer value.'),
  );

  $form['settings']['minimum_donation'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum donation'),
    '#default_value' => $settings['settings']['minimum_donation'],
    '#description' => t('Enter the minimum allowed donation as an integer. Leave blank for no minimum.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          'value' => 'select'
        ),
      ),
    ),
  );

  $form['settings']['maximum_donation'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum donation'),
    '#default_value' => $settings['settings']['maximum_donation'],
    '#description' => t('Enter the maximum allowed donation as an integer. Leave blank for no maximum.'),
    '#states' => array(
      'invisible' => array(
        ':input[name="settings[donation_entry]"]' => array(
          'value' => 'select'
        ),
      ),
    ),
  );

  // Allow other modules to add their own custom settings:
  $form['settings'] += module_invoke_all('redhen_donation_entity_settings', $settings);

  // Only show save if we're not on the field instance defaults:
  if ($entity_id) {
    $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save Settings'),
    );
  }

  return $form;
}

/**
 * Validaiton handler for redhen_donation_entity_settings_form().
 *
 * @param array $form
 * @param array $form_state
 */
function redhen_donation_entity_settings_form_validate($form, &$form_state) {
  // Extract allowed amounts
  if ($form_state['values']['settings']['donation_entry'] == 'select' || $form_state['values']['settings']['donation_entry'] == 'select_other') {
    $form_state['values']['settings']['donation_amounts'] = redhen_extract_allowed_amounts($form_state['values']['settings']['donation_amounts']);

    // Validate allowed amounts
    foreach ($form_state['values']['settings']['donation_amounts'] as $amount => $label) {
      if (!is_numeric($amount)) {
        form_set_error('settings][donation_amounts', t('Donation amounts are invalid'));
      }
    }
  }


  // Validate open date:
  if (!empty($form_state['values']['scheduling']['open']) && strtotime($form_state['values']['scheduling']['open']) === FALSE) {
    form_set_error('scheduling][open', t('Date is invalid.'));
  }

  // Validate close date:
  if (!empty($form_state['values']['scheduling']['close']) && strtotime($form_state['values']['scheduling']['close']) === FALSE) {
    form_set_error('scheduling][close', t('Date is invalid.'));
  }
}

/**
 * Submit handler for redhen_donation_entity_settings_form().
 *
 * @param array $form
 * @param array $form_state
 */
function redhen_donation_entity_settings_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];

  $fields = redhen_donation_convert_form_settings($form_state['values']);

  redhen_donation_update_entity_settings($entity['entity_type'], $entity['entity_id'], $fields);

  $uri = entity_uri($entity['entity_type'], entity_load_single($entity['entity_type'], $entity['entity_id']));

  $form_state['redirect'] = $uri['path'];
}

/**
 * Form callback: create or edit a donation.
 *
 * @param RedhenDonation $donation
 *   The donation object to edit or for a create form and empty donation
 *   object with an attached entity defined.
 */
function redhen_donation_form($form, &$form_state, RedhenDonation $donation) {
  $wrapper = entity_metadata_wrapper('redhen_donation', $donation);
  $user = $wrapper->user->value();
  $host = $wrapper->entity->value();

  $form_state['donation'] = $donation;

  $settings = redhen_donation_entity_settings($donation->entity_type, $donation->entity_id);

  if ($settings['settings']['donation_entry'] == 'select' || $settings['settings']['donation_entry'] == 'select_other') {
    $options = $settings['settings']['donation_amounts'];

    if ($settings['settings']['donation_entry'] == 'select_other') {
      $options['other'] = 'Other amount';
    }

    $form['donation_amount_select'] = array(
      '#type' => 'select',
      '#title' => t('Donation amount'),
      '#options' => $options,
      '#default_value' => $donation->amount
    );
  }

  if ($settings['settings']['donation_entry'] == 'custom' || $settings['settings']['donation_entry'] == 'select_other') {
    $form['donation_amount_other'] = array(
      '#type' => 'textfield',
      '#title' => t('Other amount'),
      '#default_value' => $donation->amount
    );

    if ($settings['settings']['donation_entry'] == 'select_other') {
      $form['donation_amount_other']['#states'] = array(
        'visible' => array(
          ':input[name="donation_amount_select"]' => array(
            'value' => 'other'
          ),
        ),
      );
    }
  }

  $donation_type = redhen_donation_type_load($donation->bundle());
  $contact_type = $donation_type->settings['contact_type'];

  // If a Contact type was found.
  if ($contact_type) {
    $contact_object = redhen_contact_create(array('type' => $contact_type));
    module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');

    // Get contact type form.
    $form['redhen_contact'] = array(
      '#type' => 'fieldset',
      '#title' => t('%type Contact information', array('%type' => ucfirst($contact_type))),
      'form' => redhen_contact_contact_form(array(), $form_state, $contact_object),
    );

    // Unset the contact forms action, we will use the registration form.
    unset($form['redhen_contact']['form']['actions']);
    // Hide the Contact email field, we will use the user mail field.
    $form['redhen_contact']['form']['redhen_contact_email']['#access'] = FALSE;
    // Add a submit handler for creating the contact.
  }

  field_attach_form('redhen_donation', $donation, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Donation'),
  );

  // Add a delete button for existing donation:
  if (isset($donation->donation_id) && entity_access('delete', 'redhen_donation', $donation)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('redhen_donation_form_delete_submit'),
      "#limit_validation_errors" => array(),
    );
  }

  if ($host) {
    $uri = entity_uri($donation->entity_type, $host);
    $form['actions']['cancel'] = array(
      '#type' => 'link',
      '#title' => t('Cancel'),
      '#href' => $uri['path'],
    );
  }

  $form['#donation_settings'] = $settings;

  return $form;
}

/**
 * Validation callback for redhen_donation_form().
 */
function redhen_donation_form_validate($form, &$form_state) {
  $donation = $form_state['donation'];

  $settings = redhen_donation_entity_settings($donation->entity_type, $donation->entity_id);

  $donation = $form_state['donation'];
  $contact_type = $form_state['redhen_contact']->bundle();
  $donation_type = redhen_donation_type_load($donation->bundle());

  // @TODO switch this to use order email or currently logged in user?
  $contact = redhen_donation_load_contact_by_current_user_or_mail($form_state['values']['mail']);
  $update_existing = $donation_type->settings['contact_update'];

  // We have an existing contact, but it's of a different type.
  if ($contact && $contact->bundle() !== $contact_type) {
    form_set_error('', t('A Contact of type %type is already associated with the email address %email.',
      array(
        '%type' => $contact->bundle(),
        '%email' => $form_state['values']['mail'],
      )
    ));
  }

  // We don't want to update contacts, but found an existing match.
  if ($contact && !$update_existing) {
    form_set_error('', t('A contact already exists with that email address.'));
  }

  // Existing contact is already linked to a user.
  if ($contact && !is_null($contact->uid) && $update_existing) {
    form_set_error('mail', t('A contact with that email address is already linked to a Drupal user.'));
  }

  // Notify field widgets to validate their data.
  field_attach_form_validate('redhen_donation', $donation, $form, $form_state);
}

/**
 * Submit callback for redhen_donation_form().
 */
function redhen_donation_form_submit($form, &$form_state) {
  global $user;
  $donation = $form_state['donation'];
  $donation_type = redhen_donation_type_load($donation->bundle());

  // Set the donation's author uid
  if (!isset($donation->donation_id)) {
    $donation->uid = $GLOBALS['user']->uid;
  }

  // Notify field widgets.
  field_attach_submit('redhen_donation', $donation, $form, $form_state);

  // Save the donation and redirect.
  if (redhen_donation_save($donation)) {
    drupal_set_message(t('Donation has been saved.'));
    $wrapper = entity_metadata_wrapper('redhen_donation', $donation);
    $host = $wrapper->entity->value();

    // Redirect to donation
    if (entity_access('view', 'redhen_donation', $donation)) {
      $uri = entity_uri('redhen_donation', $donation);
      $form_state['redirect'] = $uri['path'];
    }
    // Redirect to host entity
    else {
      if (entity_access('view', $donation->entity_type, $host)) {
        $uri = entity_uri($donation->entity_type, $host);
        $form_state['redirect'] = $uri['path'];
      }
    }

    $contact = $form_state['redhen_contact'];
    // Does a contact already exist with that email address?
    // @TODO switch this to use order email or currently logged in user?
    $existing_contact = redhen_donation_load_contact_by_current_user_or_mail($form_state['values']['mail']);

    $contact = $existing_contact ? $existing_contact : $contact;


    // Set user.
    $contact->uid = $user->uid;

    $message = t('User has been linked to the contact %name.',
      array(
        '%name' => $contact->label(),
      )
    );

    // If set to update fields, do so.
    if ($donation_type->settings['contact_update_fields'] == TRUE) {
      // Save default parameters back into the $contact object.
      $contact->first_name = $form_state['values']['first_name'];
      $contact->middle_name = $form_state['values']['middle_name'];
      $contact->last_name = $form_state['values']['last_name'];
      $contact->author_uid = $user->uid;

      // Notify field widgets.
      field_attach_submit('redhen_contact', $contact, $form, $form_state);

      // Set email address.
      redhen_contact_property_email_set($contact, NULL, $form_state['values']['mail']);
      $message .= t(' The contact was updated with the information provided.');
    }

    redhen_contact_save($contact);

    // Update form_state contact for later processing.
    $form_state['redhen_contact'] = $contact;

    drupal_set_message($message);
  }
  else {
    drupal_set_message(t('There was a problem submitting your donation.'));
  }
}

/**
 * Button submit function: handle the 'Delete' button on the node form.
 */
function redhen_donation_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $donation = $form_state['donation'];
  $form_state['redirect'] = array(
    'donation/' .
    $donation->donation_id . '/delete',
    array('query' => $destination),
  );
}
