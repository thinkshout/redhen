<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

function redhen_dedupe_merge_form($form, &$form_state, $entity_ids) {
  // parse contact ids
  // load contact entities
  $entity_ids = explode(',', $entity_ids);
  $entities = entity_load_multiple_by_name('redhen_contact', $entity_ids);
  $options = array();
  foreach ($entities as $ent_id => $entity) {
    $options[$ent_id] = $entity->first_name;
  }
  $form['Master'] = array(
    '#type' => 'radios',
    '#title' => t('Master Contact'),
    '#default_value' => 0,
    '#options' => $options,
    '#description' => t('Choose a contact to merge the other contacts into.'),
  );

  return $form;
}
