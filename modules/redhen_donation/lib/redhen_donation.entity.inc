<?php

/**
 * @file
 * Entity hooks and callbacks for donations.
 */

/**
 * Main class for donation entities.
 */
class RedhenDonation extends Entity {

  public
    $donation_id,
    $type,
    $entity_id,
    $entity_type,
    $uid = NULL,
    $order_id = NULL,
    $amount = NULL,
    $created,
    $updated;

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    $host = $wrapper->entity->value();
    if ($host) {
      return t('Donation for !title', array(
          '!title' => entity_label($this->entity_type, $host),
        )
      );
    }
    return '';
  }

  /**
   * Build content for Donation.
   *
   * @return array
   *   Render array for a donation entity.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content = parent::buildContent($view_mode, $langcode);
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    $author = $wrapper->author->value();
    $transaction_entity_wrapper = $wrapper->transaction_entity;

    $host_entity_type_info = entity_get_info($this->entity_type);
    $host_entity = $wrapper->entity->value();
    list(, , $host_entity_bundle) = entity_extract_ids($this->entity_type, $host_entity);

    $host_label = entity_label($this->entity_type, $host_entity);

    $host_uri = $host_entity ? entity_uri($this->entity_type, $host_entity) : NULL;

    // Link to host entity.
    $host_entity_link_label = (isset($host_entity_type_info['bundles'][$host_entity_bundle]['label'])) ?
      $host_entity_type_info['bundles'][$host_entity_bundle]['label']
      : '';

    $content['host_entity_link'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => $host_entity_link_label,
      '#items' => array(
        array(
          '#markup' => l($host_label, $host_uri['path']),
        ),
      ),
      '#weight' => -10,
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $contact_uri = entity_uri('redhen_contact', $wrapper->contact->value());
    $content['contact'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('RedHen Conact'),
      '#items' => array(
        array(
          '#markup' => l($wrapper->contact->full_name->value(), $contact_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );


    $author_uri = entity_uri('user', $author);
    $content['author'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Author'),
      '#items' => array(
        array(
          '#markup' => $author->uid == 0 ?
            t('Anonymous'):
            l($author->name, $author_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $content['pledged'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Pledged'),
      '#items' => array(
        array(
          '#markup' => $wrapper->pledged->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $content['received'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Received'),
      '#items' => array(
        array(
          '#markup' => $wrapper->received->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $content['updated'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Updated'),
      '#items' => array(
        array(
          '#markup' => format_date($this->updated),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $content['status'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Status'),
      '#items' => array(
        array(
          '#markup' => $wrapper->status->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
      '#attributes' => '',
    );

    $content['transaction'] = array(
      '#type' => 'fieldset',
      '#title' => t('Transaction(s)'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $rows = array();
    $header = array('order_id');
    switch ($transction_entity_type) {
      case 'commerce_recurring':
        $transaction_entity = $transaction_entity_wrapper->value();
        $transaction_entity_uri = entity_uri($transction_entity_type, $transaction_entity);
        $rows[] = array(
          l(t('Order'), $transaction_entity_uri['path']),
        );
        $transactions = array(
          'var' => array(
            '#markup' => 'Recurring'
          ),
          'stuff' => array(
            '#theme' => 'table',
            '#header' => $header,
            '#rows' => $rows,
          )

        );
        break;

      default:
        $transaction_entity = $transaction_entity_wrapper->value();
        $transaction_entity_uri = entity_uri($transction_entity_type, $transaction_entity);
        $rows[] = array(
          l(t('Order'), $transaction_entity_uri['path']),
        );
        $transactions = array(
          'var' => array(
            '#markup' => 'ONE TIME'
          ),
          'stuff' => array(
            '#theme' => 'table',
            '#header' => $header,
            '#rows' => $rows,
          )

        );
        break;
    }
    $content['transaction']['list'] = $transactions;



    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Save donation.
   *
   * @see entity_save()
   */
  public function save() {
    $this->updated = REQUEST_TIME;

    if (!$this->donation_id && empty($this->created)) {
      $this->created = REQUEST_TIME;
    }
    return parent::save();
  }

  /**
   * Specify URI.
   */
  protected function defaultUri() {
    return array('path' => 'redhen/donation/' . $this->internalIdentifier());
  }

}

