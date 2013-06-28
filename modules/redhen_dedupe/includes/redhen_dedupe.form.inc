<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

/**
 * Form to select dedupe options.
 */
function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // parse contact ids
  // load contact entities
  $entity_ids = explode(',', $entity_ids);
  $entities = entity_load('redhen_contact', $entity_ids);
  $wrappers = array();
  foreach ($entities as $ent_id => $entity) {
    $wrappers[$ent_id] = entity_metadata_wrapper('redhen_contact', $entity);
  }
  $table_rows = array();
  $fields = field_info_instances('redhen_contact');
  $table_header = array('Field Name');
  $options = array();
  $bundles_used = array();
  foreach ($wrappers as $ent_id => $wrapper) {
    $updated = format_date($wrapper->updated->value(), 'short');
    $options[$ent_id] = $wrapper->full_name->value() . ' (Updated: ' . $updated . ')';
    $table_header[] = $wrapper->full_name->value() . ' (Updated: ' . $updated . ')';
    $bundles_used[] = $wrapper->getBundle();
  }
  foreach ($bundles_used as $bundle) {
    foreach ($fields[$bundle] as $fieldname => $field) {
      $table_rows[$fieldname] = array($field['label']);
    }
  }
  $form['master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#options' => $options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
  );
  $form['entities'] = array('#type' => 'hidden', '#value' => $entities);
  $form['contact_previews'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview'),
  );
  $views = entity_view('redhen_contact', $entities);
  foreach ($views['redhen_contact'] as $ent_id => $entity_view) {
    foreach ($table_rows as $name => &$row) {
      $value = render($entity_view[$name]);
      $row[] = $value;
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
    '#rows' => $table_rows,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Merge'),
  );

  return $form;
}

/**
 * Submit handler for dedupe merge form.
 */
function redhen_dedupe_merge_form_submit($form, &$form_state) {
  $master_id = $form_state['values']['master'];
  $master = redhen_contact_load($master_id);
  $merge_status = redhen_dedupe_merge($master, $form_state['values']['entities'], $form_state['values']['values'], $form_state['values']['related_entities']);
  if ($merge_status) {
    drupal_set_message('Contacts merged.');
  }
}


function redhen_dedupe_merge_details_form($form, &$form_state) {
  
}
