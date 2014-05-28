<?php

/**
 * Page callback: Add a new donation to a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_donate_page_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donate_page($entity_type, $entity) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  if (redhen_donation_status($entity_type, $entity_id)) {
    $donation_type = redhen_donation_get_entity_donation_type($entity_type, $entity);
    $donation = entity_get_controller('redhen_donation')->create(array(
      'entity_type' => $entity_type,
      'entity_id' => $entity_id,
      'type' => $donation_type,
    ));
    return drupal_get_form('redhen_donation_form', $donation);
  }
  else {
    return t('Sorry, donations are no longer available for %name',
      array('%name' => entity_label($entity_type, $entity)));
  }
}

/**
 * Page callback for entity donation settings.
 *
 * @param $entity_type
 * @param $entity
 *
 * @return array
 *   Donation entity settings form.
 */
function redhen_donation_entity_settings_page($entity_type, $entity) {
  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $settings = redhen_donation_entity_settings($entity_type, $entity_id);
  return drupal_get_form('redhen_donation_entity_settings_form', $settings, $entity_type, $entity_id);
}

/**
 * Page callback: Show a list of donations for a host entity.
 *
 * @param string $entity_type
 *   The host entity type.
 * @param object $entity
 *   The host entity.
 *
 * @return array
 *   A render array
 *
 * @see redhen_donation_administer_donations_access()
 * @see redhen_donation_menu()
 */
function redhen_donation_donations_page($entity_type, $entity) {
  $header = array(
    array(
      'data' => t('ID'),
      'field' => 'donation_id',
      'type' => 'property',
      'specifier' => 'donation_id'
    array(
      'data' => t('Author'),
    ),
    array(
      'data' => t('Contact'),
    ),
    array(
      'data' => t('Amount Pledged'),
    ),
    array(
      'data' => t('Amount Received'),
    ),
    array(
      'data' => t('Updated'),
      'field' => 'updated',
      'sort' => 'desc',
      'type' => 'property',
      'specifier' => 'updated'
    ),
    array(
      'data' => t('Status'),
      'specifier' => 'status'
    ),
    array(
      'data' => t('Recurring/Order'),
    ),
  );

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = entity_label($entity_type, $entity);

  $query = new EntityFieldQuery;
  $result = $query
    ->entityCondition('entity_type', 'redhen_donation')
    ->propertyCondition('entity_id', $entity_id)
    ->propertyCondition('entity_type', $entity_type)
    ->pager(20)
    ->tableSort($header)
    ->execute();

  if (!empty($result['redhen_donation'])) {
    $donations = redhen_donation_load_multiple(array_keys($result['redhen_donation']));
    $rows = array();

    foreach ($donations as $donation) {
      $wrapper = entity_metadata_wrapper('redhen_donation', $donation);

      if ($donation->author_uid) {
        $user = user_load($donation->author_uid);
        $uri = entity_uri('user', $user);
        $user_col = l($user->name, $uri['path']);
      }
      else {
        $user_col = t('Anonymous');
      }

      $transaction_entity_wrapper = $wrapper->transaction_entity;
      $transaction_entity = $transaction_entity_wrapper->value();
      $transaction_entity_type = $wrapper->transaction_entity_type->value();
      $transaction_entity_uri = entity_uri($transaction_entity_type,
        $transaction_entity);
      $transaction_entity_label = entity_label($transaction_entity_type, $transaction_entity);
      $rows[] = array(
        l($donation->donation_id, 'redhen/donation/' . $donation->donation_id),
        $user_col,
        $wrapper->pledged->value(),
        $wrapper->received->value(),
        l($wrapper->contact->label(), $wrapper->contact->value()->uri()['path']),
        format_date($donation->updated),
        $wrapper->status->value(),
        l(
          $transaction_entity_label ?: t('Recurring') . ' ' .
           $donation->transaction_entity_id,
          $transaction_entity_uri['path']
        ),
      );
    }

    $table = array(
      'header' => $header,
      'rows' => $rows
    );

    $out = theme('table', $table) . theme('pager');
  }
  else {
    $out = t('There are no donations for %name',
      array('%name' => $label));
  }

  return $out;
}

/**
 * RedHen Donation view.
 */
function redhen_donation_view(RedhenDonation $donation, $view_mode = 'full') {
  return $donation->view($view_mode);
}
