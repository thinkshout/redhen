<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Form to select the master contact.
 */
function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // Load the entities we want to merge:
  $entity_ids = explode(',', $entity_ids);
  $entities = redhen_contact_load_multiple($entity_ids);

  $master_options = array();

  // Loop through the entities to build out our master entity options:
  foreach ($entities as $ent_id => $entity) {
    $wrapper = entity_metadata_wrapper('redhen_contact', $entity);
    $updated = format_date($wrapper->updated->value(), 'short');
    $table_header[] = $master_options[$ent_id] = t('@name (Updated: !date)', array('!date' => $updated, '@name' => $wrapper->label()));
    $form['contact_previews'][$ent_id] = array(
      '#type' => 'container',
      'contact' => $wrapper->view(),
      '#states' => array(
        'visible' => array(
          ':input[name="master"]' => array('value' => $ent_id),
        ),
      ),
      '#weight' => 99,
    );
  }

  // Form field to select a merge master entity.
  $form['master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#required' => TRUE,
    '#options' => $master_options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
    '#weight' => 0,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  return $form;
}

/**
 * Submit handler for redhen_dedupe_merge_master form.
 */
function redhen_dedupe_merge_form_submit($form, &$form_state) {
  // We hand off the the merge details from with 2 arguments: our master list ID
  // and our full list of IDs (which includes the master).
  $master = $form_state['values']['master'];
  $all = implode(',', array_flip($form['master']['#options']));
  $form_state['redirect'] = 'redhen/dedupe/merge_details/' . $master . '/' . $all;
}

/**
 * Form to select dedupe options.
 */
function redhen_dedupe_merge_details_form($form, &$form_state, $master_id, $entity_ids) {
  // Load the entities we want to merge:
  $entity_ids = explode(',', $entity_ids);
  $contacts = redhen_contact_load_multiple($entity_ids);

  // Initialize our table header:
  $table_header = array(t('Field Name'));
  // Loop through the entities to build out our table headers and master
  // entity options:
  foreach ($contacts as $ent_id => $contact) {
    $wrapper = entity_metadata_wrapper('redhen_contact', $contact);
    $updated = format_date($wrapper->updated->value(), 'short');
    $table_header[$ent_id] = array(
      'data' => t('@name (@bundle)<br/>Last Updated: !date', array('!date' => $updated, '@name' => $wrapper->label(), '@bundle' => $wrapper->getBundle())),
      'class' => ($ent_id == $master_id) ? 'redhen-dedupe-master-col' : 'redhen-dedupe-col',
    );
  }

  // Pass along the entity ID options & master ID to the form handler:
  $form_state['master'] = $master_id;
  $form_state['contacts'] = $contacts;

  // Because of duplicate values and the complicated data selections, we put
  // all the actual data into a separate data array, and use the form fields
  // to index into it. (This allows us to generate some more complex form
  // field values here, like merging fields.)
  $values = array();
  // This v_index will be the unique key that we place as the form value, and
  // use to index into the values array later to grab the actual data.
  $v_index = 0;

  // Now we build our merge selector form fields:
  $form['values'] = array(
    '#theme' => 'redhen_dedupe_form_table',
    '#tree' => TRUE,
    '#header' => $table_header,
  );
  $info = entity_get_property_info('redhen_contact');
  $properties = entity_get_all_property_info('redhen_contact');
  // Loop through each property and build a form element for it. The form
  // element will be placed into a table in redhen_dedupe_form_table:
  foreach ($properties as $name => $property) {
    // Call a helper function to determine if this is a field we want to merge:
    if (redhen_dedupe_property_mergeable($name, $property)) {
      $form['values'][$name] = array(
        '#type' => 'radios',
        '#title' => $property['label'],
        '#options' => array(),
        // We set a flag to determine if this field is used at all, so we
        // can unset it later if it is not used by these redhen_contact bundles.
        'in_use' => FALSE,
      );
      $options = &$form['values'][$name]['#options'];
      $is_field = isset($properties[$name]['field']) && $properties[$name]['field'];
      // Loop through each contact to build a row element/radio button option:
      foreach ($contacts as $ent_id => $contact) {
        // We do some work to figure out what kind of field we are dealing with,
        // and set our values and displays appropriately. The important factors
        // are if it's a field or not, and whether it has a setter/getter
        // callback that we should be using.
        $valid_property = TRUE;
        $in_bundle = isset($info['bundles'][$contact->type]['properties'][$name]);
        if (!$in_bundle && $is_field) {
          $valid_property = FALSE;
        }

        if (!$valid_property) {
          $values[$v_index] = NULL;
          $display = t('Field not applicable');
        }
        else {
          $form['values'][$name]['in_use'] = TRUE;
          // Set the default to match the Master record:
          if ($ent_id == $master_id) {
            $form['values'][$name]['#default_value'] = $v_index;
          }
          if (isset($properties[$name]['getter callback'])) {
            $wrapper = entity_metadata_wrapper('redhen_contact', $entity);
            $values[$v_index] = $wrapper->{$name}->value();
          }
          else {
            $values[$v_index] = $entity->{$name};
          }
          // Generate a view array to see if we have a nice display to use for
          // this element:
          $views_array = redhen_contact_view($entity);
          $entity_view = $views_array['redhen_contact'][$ent_id];
          if (isset($entity_view[$name])) {
            $display = render($entity_view[$name]);
          }
          else {
            $display = $values[$v_index];
          }
        }

        $options[$v_index] = $display;
        $v_index++;
      }
    }
  }

  // Stash all the actual data in the form for later processing:
  $form_state['field_data'] = $values;

  // Unset any rows that had no valid values:
  // @todo: hide or disable elements where the values are equivalent.
  foreach (element_children($form['values']) as $row) {
    if ($form['values'][$row]['in_use']) {
      unset($form['values'][$row]['in_use']);
    }
    else {
      unset($form['values'][$row]);
    }
  }

  $form['dead_contact_fate'] = array(
    '#type' => 'radios',
    '#title' => t('What do you want to do with the old records?'),
    '#default_value' => 1,
    '#options' => array(0 => t('Delete'), 1 => t('Archive')),
  );

  $related_types = array();
  if (module_exists('redhen_note')) {
    $related_types['redhen_note'] = t('Notes');
  }
  if (module_exists('redhen_engagement')) {
    $related_types['redhen_engagement'] = t('Engagement Scores');
  }
  if (module_exists('redhen_membership')) {
    $related_types['redhen_membership'] = t('Memberships');
  }
  if (module_exists('redhen_relation')) {
    $related_types['relation'] = t('Relationships/Affiliations');
  }
  if (count($related_types) > 0) {
    $form['related_entities'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Move items attached to old records to Master record:'),
      '#options' => $related_types,
      '#default_value' => array_keys($related_types),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Merge'),
  );

  return $form;
}

/**
 * Submit handler for dedupe merge form.
 */
function redhen_dedupe_merge_details_form_submit($form, &$form_state) {
  $master_id = $form_state['master'];
  $contacts = $form_state['contacts'];
  $master = redhen_contact_load($master_id);
  // Pull the actual data out of the #value array constructed for the form:
  foreach ($form_state['values']['values'] as $name => $val) {
    $form_state['values']['values'][$name] = $form_state['field_data'][$val];
  }
  $merge_status = redhen_dedupe_merge($master, $contacts, $values, $form_state['values']['related_entities'], $form_state['values']['dead_contact_fate']);
  if ($merge_status) {
    drupal_set_message('Contacts merged.');
  }
}


/**
 * Themes the field associations on a fieldmap edit form into a table.
 */
function theme_redhen_dedupe_form_table($variables) {
  $elements = $variables['elements'];

  // Build the rows array.
  $rows = array();
  foreach (element_children($elements) as $item_key) {
    $item = &$elements[$item_key];
    $data = array($item['#title']);
    foreach (element_children($item) as $element) {
      $cell = array('data' => drupal_render($item[$element]));
      if (isset($item[$element]['#attributes'])) {
        foreach ($item[$element]['#attributes'] as $key => $value) {
          $cell[$key] = ($key == 'id') ? (is_array($value) ? array($value[0] . '-cell') : $value . '-cell') : $value;
        }
      }
      $data[] = $cell;

    }
    $row = array('data' => $data);
    if (isset($item_key['#attributes'])) {
      foreach ($item_key['#attributes'] as $key => $value) {
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
 * Determine if a given property can be merged.
 *
 * @param array $property
 *   A property as returned by entity_get_all_property_info();
 *
 * @return bool
 *   True is mergeable.
 */
function redhen_dedupe_property_mergeable($name, $property) {

  // Don't merge computed fields:
  if (isset($property['computed']) && $property['computed']) {
    return FALSE;
  }

  // Don't merge fields that can't be edited:
  if (!isset($property['setter callback'])) {
    return FALSE;
  }

  // Don't merge any base table fields except the name fields:
  $info = entity_get_info('redhen_contact');
  if (in_array($name, $info['schema_fields_sql']['base table'])) {
    if (!strstr($name, '_name')) {
      return FALSE;
    }
  }

  return TRUE;
}
