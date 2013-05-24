<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // parse contact ids
  // load contact entities
  $entity_ids = explode(',', $entity_ids);
  $entities = entity_load('redhen_contact', $entity_ids);
  $options = array();
  foreach ($entities as $ent_id => $entity) {
    $wrapper = entity_metadata_wrapper('redhen_contact', $entity);
    $updated =  format_date($wrapper->updated->value(), 'short');
    $options[$ent_id] = $wrapper->full_name->value() . ' (' . $updated . ')';
    $form[$ent_id] = array(
      '#markup' => drupal_render(entity_view('redhen_contact', array($entity))),
      '#states' => array(
        'visible' => array(
          ':input[name="master"]' => array('value' => $ent_id),
        ),
      ),
      '#weight' => 99,
    );
  }
  $form['master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#options' => $options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Merge'),
  );

  return $form;
}
