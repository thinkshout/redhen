<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Form to select dedupe options.
 */
function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // Load the entities we want to merge:
  $entity_ids = explode(',', $entity_ids);
  $entities = redhen_contact_load_multiple($entity_ids);

  // Create entity metadata wrappers for all entities. We'll need these.
  $wrappers = array();
  foreach ($entities as $ent_id => $entity) {
    $wrappers[$ent_id] = entity_metadata_wrapper('redhen_contact', $entity);
  }

  // Initialize our table of fields for the table of value selectors:
  $field_table = array();
  $fields = field_info_instances('redhen_contact');

  // Initialize our table header:
  $table_header = array('Field Name');
  $master_options = array();

  // We tally every bundle used by any of the entities so we know what fields
  // to look at later:
  $bundles_used = array();

  // Loop through the entities to build out our table headers and master
  // entity options:
  foreach ($wrappers as $ent_id => $wrapper) {
    $updated = format_date($wrapper->updated->value(), 'short');
    $table_header[] = $master_options[$ent_id] = t('@name (Updated: !date)', array('!date' => $updated, '@name' => $wrapper->label()));
    $bundles_used[$wrapper->getBundle()] = $wrapper->getBundle();
  }

  // Use the list of bundles to build a list of fields to merge:
  foreach ($bundles_used as $bundle) {
    foreach ($fields[$bundle] as $fieldname => $field) {
      $field_table[$fieldname] = array($field['label']);
    }
  }

  // Pass along the entity ID options to the form handler:
  $form['entities'] = array('#type' => 'hidden', '#value' => $entity_ids);

  // Form field to select a merge master entity.
  $form['master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#options' => $master_options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
  );
  $form['contact_previews'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
  );

  // We build an array of views so we can pull out rendered values into our
  // selection table:
  $views = entity_view('redhen_contact', $entities);
  foreach ($views['redhen_contact'] as $ent_id => $entity_view) {
    foreach ($field_table as $name => &$row) {
      $row[] = render($entity_view[$name]);
    }
    $form['contact_previews'][$ent_id] = array(
      '#type' => 'container',
      'contact' => $entity_view,
      '#states' => array(
        'visible' => array(
          ':input[name="master"]' => array('value' => $ent_id),
        ),
      ),
      '#weight' => 99,
    );
  }

  $form['field_selectors'] = array(
    '#theme' => 'table',
    '#header' => $table_header,
    '#rows' => $field_table,
  );

  $form['dead_contact_fate'] = array(
    '#type' => 'radios',
    '#title' => t('What do you want to do with the old records?'),
    '#default_value' => 1,
    '#options' => array(0 => t('Delete'), 1 => t('Archive')),
  );
  $form['related_entity_action'] = array(
    '#type' => 'radios',
    '#title' => t('What do you want to do with notes and other items attached to the old records?'),
    '#default_value' => 1,
    '#options' => array(0 => t('Delete'), 1 => t('Move to new Master')),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Merge'),
  );

  return $form;
}


function redhen_dedupe_merge_form_submit($form, &$form_state) {
  // redhen/dedupe/merge/' . $result->ids),
}


function redhen_dedupe_merge_details_form($form, &$form_state) {
  
}
