<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;
use Drupal\Core\Url;

/**
 * Provides a listing of Connection type entities.
 */
class ConnectionTypeListBuilder extends ConfigEntityListBuilder {
  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Connection type');
    $header['endpoint_1'] = $this->t('Endpoint 1');
    $header['endpoint_2'] = $this->t('Endpoint 2');
    $header['id'] = $this->t('Machine name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $entity->label();
    $row['endpoint_1'] = $entity->getEndPointLabel(1);
    $row['endpoint_2'] = $entity->getEndPointLabel(2);
    $row['id'] = $entity->id();
    return $row + parent::buildRow($entity);
  }

  /**
   * {@inheritdoc}
   */
  public function getDefaultOperations(EntityInterface $entity) {
    $operations = parent::getDefaultOperations($entity);

    if ($entity->access('update') && $entity->hasLinkTemplate('edit-form')) {
      $operations['roles'] = array(
        'title' => $this->t('Connection Roles'),
        'weight' => 100,
        'url' => $url = Url::fromRoute('entity.redhen_connection_role.collection', ['redhen_connection_type' => $entity->id()]),
      );
    }

    return $operations;
  }


}
