<?php

/**
 * Return a form for an entity's donation settings.
 *
 * @param array $form
 * @param array $form_state
 * @param $settings
 * @param object $entity_type
 * @param null $entity_id
 *
 * @internal param object $entity
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

  if (!isset($form_state['methods'])) {
    $form_state['methods'] = $settings['settings']['commerce']['payment_options']['payment_methods']?:array();
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
    '#collapsed' => TRUE,
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
    '#default_value' => $settings['settings']['donation_settings']['donation_entry'],
    '#options' => array(
      'select' => t('Select list'),
      'select_other' => t("Select list (with 'other' option)"),
      'radio_other' => t("Radio options (with 'other' option)"),
      'custom' => t('Text field (custom amount)')),
  );

  $form['settings']['donation_amounts'] = array(
    '#type' => 'textarea',
    '#title' => t('Donation amounts'),
    '#default_value' => redhen_allowed_amounts_string($settings['settings']['donation_settings']['donation_amounts']),
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
    '#default_value' => $settings['settings']['donation_settings']['minimum_donation'],
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
    '#default_value' => $settings['settings']['donation_settings']['maximum_donation'],
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

  $fields = field_info_instances('redhen_contact', 'general');

  $address_fields = array(
    'Commerce' => array('billing'=> 'Billing information')
  );
  $other_fields = array();
  foreach ($fields as $field_name => $field) {
    $field_info = field_info_field($field_name);
    if ($field_info['type'] == 'addressfield') {
      $address_fields['RedHen'][$field_name] = $field['label'];
    }
    else {
      if ($field_info['type']) {
        $other_fields[$field_name] = $field['label'];
      }

    }
  }

  $form['redhen_field_selector'] = array(
    '#type' => 'fieldset',
    '#title' => t('RedHen Field Selector'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );
  $other_fields['middle_name'] = 'Middle Name';
  $form['redhen_field_selector']['fields'] = array(
    '#type' => 'checkboxes',
    '#options' => $other_fields,
    '#default_value' => $settings['settings']['redhen_fields']['fields'],
  );

  $form['commerce'] = array(
    '#type' => 'fieldset',
    '#title' => t('Commerce Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
  );

  $form['commerce']['profile_selector'] = array(
    '#type' => 'select',
    '#title' => t('Profile field'),
    '#options' => $address_fields,
    '#default_value' => $settings['settings']['commerce']['profile_selector'],
  );

  $form['commerce']['payment_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Payment Methods'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#id' => 'edit-redhen-donation-payment-options-wrapper',
  );

  $form['commerce']['payment_options']['payment_methods'] = array(
    '#theme' => 'redhen_donation_payment_options_form_table',
    '#tree' => TRUE,
    '#header' => array(
      'one_time' => t('Payment method'),
      'label' => t('Label'),
      'currency' => t('Currency'),
      'frequency' => t('Frequency'),
      'delete' => t('Delete'),
    ),
    '#attributes' => array(
      'id' => array('edit-redhen-donation-payment-options'),
    ),
  );

  // Get Payment Methods
  $conditions = array('event' => 'commerce_payment_methods', 'plugin' => 'reaction rule', 'active' => TRUE);
  $payment_rules = rules_config_load_multiple(FALSE, $conditions);
  foreach ($payment_rules as $name => $rule) {
    foreach ($rule->actions() as $action) {
      $method = isset($action->settings['payment_method']['method_id'])?
      $action->settings['payment_method']['method_id']:
      FALSE;
    }
    if ($method) {
      $payment_options[$method . '|'. $name] = $rule->label;
    }
  }

  $methods = redhen_donation_get_payment_methods($form_state);

  foreach ($methods as $delta => $value) {
    $row_id = 'edit-payment-methods-' . $delta;
    $form['commerce']['payment_options']['payment_methods'][$delta] = array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => array($row_id),
      ),
    );
    $row = &$form['commerce']['payment_options']['payment_methods'][$delta];

    $row['one_time'] = array(
      '#type' => 'select',
      '#options' => $payment_options,
      '#default_value' => $form_state['methods'][$delta]['one_time'],
    );
    $row['label'] = array(
      '#type' => 'textfield',
      '#size' => 2,
      '#default_value' => $form_state['methods'][$delta]['label'],
    );
    $row['currency'] = array(
      '#type' => 'textfield',
      '#size' => 2,
      '#default_value' => $form_state['methods'][$delta]['currency'],
    );
    $row['delete'] = array(
      '#id' => 'edit-delete-' . $delta,
      '#type' => 'checkbox',
      '#value' => t('Delete'),
      '#name' => 'delete_method-' . $delta,
      '#ajax' => array(
        'callback' => 'redhen_donation_settings_form_callback',
        'wrapper' => 'edit-redhen-donation-payment-options-wrapper',
        'delta' => $delta,
      ),
    );
  }

  $form['commerce']['payment_options']['add_field'] = array(
    '#value' => t('Add another payment method'),
    '#id' => 'edit-donation-add-payment-method',
    '#name' => 'donation_add_field',
    '#type' => 'button',
    '#description' => t('Add one or more payment methods.'),
    '#executes_submit_callback' => FALSE,
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'redhen_donation_settings_form_callback',
      'wrapper' => 'edit-redhen-donation-payment-options-wrapper',
    ),
  );

  // Product options
  $rec_product_options = array(
    '' => t('None')
  );
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'commerce_product')
     ->propertyCondition('status', 1)
   ;
  $result = $query->execute();
  $products = commerce_product_load_multiple(array_keys($result['commerce_product']));
  foreach ($products  as $id => $product) {
    if ($product->type == 'recurring') {
      $rec_product_options[$id] = $product->title;
    }
    else {
      $product_options[$id] = $product->title;
    }

  }

  $form['commerce']['one_time_product'] = array(
    '#type' => 'select',
    '#title' => t('One Time Product'),
    '#options' => $product_options,
  );

  $form['commerce']['recurring'] = array(
    '#type' => 'select',
    '#title' => t('Recurring Product'),
    '#options' => $rec_product_options,
  );

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
 * Donation settings form AJAX callback.
 *
 * @param $form
 * @param $form_state
 *
 * @return array
 */
function redhen_donation_settings_form_callback($form, $form_state) {
  $trigger = $form_state['triggering_element']['#name'];
  $parents = $form_state['triggering_element']['#array_parents'];
  $delta = isset($parents[3]) ? $parents[3]: NULL;
  $commands = array();
  switch ($trigger) {

    case 'donation_add_field':
    case 'delete_method-' . $delta:
      // Replace the field map table.
      $commands = array(
        ajax_command_replace('#edit-redhen-donation-payment-options-wrapper', render($form['commerce']['payment_options'])),
      );
      break;
  }
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Validaiton handler for redhen_donation_entity_settings_form().
 *
 * @param array $form
 * @param array $form_state
 */
function redhen_donation_entity_settings_form_validate($form, &$form_state) {
  // Extract allowed amounts
  if ($form_state['values']['settings']['donation_entry'] == 'select' ||
    $form_state['values']['settings']['donation_entry'] == 'select_other' ||
    $form_state['values']['settings']['donation_entry'] == 'radio_other') {
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
  $form_state['values']['methods'] = $form_state['methods'];

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

  $donation_type = redhen_donation_type_load($donation->bundle());
  $contact_type = $donation_type->settings['contact_type'];

  // If a Contact type was found.
  if ($contact_type) {
    $contact_object = redhen_contact_create(array('type' => $contact_type));
    module_load_include('inc', 'redhen_contact', 'includes/redhen_contact.forms');

    // Get contact type form.
    $form['redhen_contact'] = array(
      '#type' => 'fieldset',
      '#title' => t('Contact information'),
      'form' => redhen_contact_contact_form(array(), $form_state, $contact_object),
    );

    // Unset the contact forms action, we will use the registration form.
    unset($form['redhen_contact']['form']['actions']);
    // Hide the Contact email field, we will use the user mail field.
    $form['redhen_contact']['form']['redhen_contact_email']['#access'] = FALSE;
    // Add a submit handler for creating the contact.

    // Remove fields as defined in settings
    $fields = field_info_instances('redhen_contact', 'general');
    foreach ($fields as $field) {
      $field_info = field_info_field($field);
      $field_setting = $settings['settings']['redhen_fields']['fields'][$field_info['field_name']];
      if ($field_info['type'] == 'addressfield' || !$field_setting) {
        $form['redhen_contact']['form'][$field_info['field_name']]['#access'] = FALSE;
      }

    }
    if (!$settings['settings']['redhen_fields']['fields']['middle_name']) {
      $form['redhen_contact']['form']['name']['middle_name']['#access'] = FALSE;
    }
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
  }

  // TODO: Handle currency mapping.
  $cur = '$';
  foreach ($settings['settings']['donation_settings']['donation_amounts'] as $amount) {
    $donation_ammounts[$amount] =  $cur . $amount;
  }

  switch ($settings['settings']['donation_settings']['donation_entry']) {
    case 'select':
      $form['donation'] = array(
        '#type' => 'select',
        '#title' => t('Donation'),
        '#required' => TRUE,
        '#options' => $donation_ammounts,
        '#multiple' => FALSE,
      );
      break;

    case 'select_other':
    case 'radio_other':
      $other = t('Other (please type an amount in @cur)', array('@cur' => $cur));

      $form['donation'] = array(
        '#type' => 'select_or_other',
        '#title' => t('Donation'),
        '#required' => TRUE,
        '#options' => $donation_ammounts,
        '#other' => $other,
        '#multiple' => FALSE,
        '#other_unknown_defaults' => 'other',
        '#other_delimiter' => ', ',
        '#select_type' =>
          $settings['settings']['donation_entry'] == 'select_other' ?
          'select' : 'radios',
      );

      // TODO: Handle other amount with non-numeric cahr
      break;

    default:
      $form['donation'] = array(
        '#type' => 'textfield',
        '#title' => t('Amount'),
        '#default_value' => $donation
      );
      break;
  }

  if ($settings['settings']['commerce']['recurring'] != '') {
    $form['frequency'] = array(
      '#title'=> t('Frequency'),
      '#type' => 'select',
      '#options' => array(
        'one_time' => t('One time'),
        'recurring' => t('Recurring'),
      ),
    );
  }

  $form['#donation_settings'] = $settings;

  $billing_address_field = $settings['settings']['commerce']['profile_selector'];
  if ($billing_address_field == 'billing') {
    $billing = commerce_customer_profile_new('billing', $user->uid);
    $form_state['billing_address'] = $billing;
    field_attach_form('commerce_customer_profile', $billing, $form, $form_state);
  }
  else {
    $form['billing_address'] = $form['redhen_contact']['form'][$billing_address_field];
    $form['billing_address']['#access'] = TRUE;
  }
  $form['billing_address']['#weight'] = 1;


  $payment_settings = $settings['settings']['commerce']['payment_options']['payment_methods'];
  $payment_methods = array();
  foreach ($payment_settings as $payment_setting) {
    $payment_methods[$payment_setting['one_time']] = commerce_payment_method_instance_load($payment_setting['one_time']);
  }
  $options = array();

  $payment_methods_new = $settings['settings']['commerce']['payment_options']['payment_methods'];
  foreach ($payment_methods_new as $payment_method_new) {
    $options[$payment_method_new['one_time']] = $payment_method_new['label'];
  }

  // Add a radio select widget to specify the payment method.
  $pane_form['payment_method'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#ajax' => array(
      'callback' => 'redhen_donation_form_refresh',
      'wrapper' => 'payment-details',
    ),
    '#limit_validation_errors' => array(),
  );

  if (isset($form_state['values'])) {
    $default_method = $form_state['values']['payment_method'];
  }
  else {
    reset($options);
    $default_method = key($options);
  }

  // Set the default value for the payment method radios.
  $pane_form['payment_method']['#default_value'] = $default_method;

  $payment_method = $payment_methods[$default_method];

  $checkout_pane = array();
  $order = new stdClass();
  $pane_values = array();
  if ($callback = commerce_payment_method_callback($payment_method, 'submit_form')) {
    $pane_form['payment_details'] = $callback($payment_method, $pane_values, $checkout_pane, $order);
  }
  else {
    $pane_form['payment_details'] = array();
  }

  $pane_form['payment_details']['#tree'] = TRUE;
  $pane_form['payment_details']['#prefix'] = '<div id="payment-details">';
  $pane_form['payment_details']['#suffix'] = '</div>';

  $form['commerce_payment_form'] = array(
    '#type' => 'fieldset',
    '#weight' => 2,
    '#title' => t('Payment Information'),
  );

  $form['commerce_payment_form'] = array_merge($form['commerce_payment_form'], $pane_form);

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

/**
 * Themes the payment methods on a fieldmap edit form into a table.
 */
function theme_redhen_donation_payment_options_form_table($variables) {
  $elements = $variables['elements'];

  // Build the rows array.
  $columns = isset($elements['#columns']) ? $elements['#columns'] : (isset($elements['#header']) ? array_keys($elements['#header']) : array());
  $rows = array();
  foreach (element_children($elements) as $child_key) {
    $child = & $elements[$child_key];
    $data = array();
    $row_columns = empty($columns) ? element_children($child) : $columns;
    foreach ($row_columns as $column) {
      if (isset($child[$column])) {
        $cell = array('data' => drupal_render($child[$column]));
        if (isset($child[$column]['#attributes'])) {
          foreach ($child[$column]['#attributes'] as $key => $value) {
            $cell[$key] = ($key == 'id') ? (is_array($value) ? array($value[0] . '-cell') : $value . '-cell') : $value;
          }
        }
        $data[] = $cell;
      }
    }
    $row = array('data' => $data);
    if (isset($child['#attributes'])) {
      foreach ($child['#attributes'] as $key => $value) {
        $row[$key] = $value;
      }
    }
    $rows[] = $row;
  }

  $config = array(
    'rows' => $rows,
  );
  if (isset($elements['#header'])) {
    $config['header'] = $elements['#header'];
  }
  if (isset($elements['#attributes']) && is_array($elements['#attributes'])) {
    $config['attributes'] = $elements['#attributes'];
  }

  return theme('table', $config);
}

/**
 * Payment method selector helper.
 *
 * @param $form_state
 * @param null $delta
 *
 * @return array|null
 */
function redhen_donation_get_payment_methods(&$form_state, $delta = NULL) {
  $methods = NULL;
  if (isset($form_state['methods'])) {
    $methods = &$form_state['methods'];
  }

  $value = NULL;

    if (empty($methods)) {
      $methods = array(redhen_donation_get_empty_field_map_row());
    }
    if (isset($form_state['triggering_element'])) {
      if ($form_state['triggering_element']['#name'] == 'donation_add_field') {
        $methods[] = redhen_donation_get_empty_field_map_row();
      }
      if (isset($form_state['triggering_element']['#ajax']['delta']) && $form_state['triggering_element']['#name'] == 'delete_method-' . $form_state['triggering_element']['#ajax']['delta']) {
        $delta = $form_state['triggering_element']['#ajax']['delta'];
        unset($methods[$delta]);
      }
    }
    $value = &$methods;



  return $value;
}


/**
 * Helper to rapidly retreive an empty field mapping array.
 *
 * @return array
 *   An empty field mapping array.
 */
function redhen_donation_get_empty_field_map_row() {
  return array();
}


/**
 * Donation form AJAX callback.
 *
 * @param $form
 * @param $form_state
 *
 * @return array
 */
function redhen_donation_form_refresh(&$form, &$form_state) {
  $html1 = drupal_render($form['commerce_payment_form']['payment_details']);
  $html2 = drupal_render($form['donation']);
  $result = array(
      '#type' => 'ajax',
      '#commands' => array(
        ajax_command_replace('#payment-details', $html1),
        ajax_command_replace('.form-item-donation', $html2),
      ),
    );
  return $result;
}