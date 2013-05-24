<?php

/**
 * @file
 * Forms for creating, editing, and deleting contacts.
 */

function redhen_dedupe_merge_form($form, &$form_state) {
  // parse contact ids
  // load contact entities
  $entities = array();
  foreach ($form_state['build_info']['args'] as $id) {
    $matches = entity_load('redhen_contact', array($id));
    foreach ($matches as $match_id => $match) {
      $entities[$match_id] = $match;
    }
  }
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
}
