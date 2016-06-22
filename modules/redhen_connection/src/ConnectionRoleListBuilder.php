<?php

namespace Drupal\redhen_connection;

use Drupal\Core\Config\Entity\ConfigEntityListBuilder;
use Drupal\Core\Entity\EntityInterface;

/**
 * Provides a listing of Connection Role entities.
 */
class ConnectionRoleListBuilder extends ConfigEntityListBuilder {

  /**
   * {@inheritdoc}
   */
  protected function getEntityIds() {

    $query = $this->getStorage()->getQuery()
      ->sort($this->entityType->getKey('id'));

    if ($connection_type = \Drupal::routeMatch()->getParameter('redhen_connection_type')) {
      $query->condition('connection_type',$connection_type->id(),'=');
    }

    if ($this->limit) {
      $query->pager($this->limit);
    }
    return $query->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function buildHeader() {
    $header['label'] = $this->t('Connection Role');
    $header['id'] = $this->t('Machine name');
    return $header + parent::buildHeader();
  }

  /**
   * {@inheritdoc}
   */
  public function buildRow(EntityInterface $entity) {
    $row['label'] = $entity->label();
    $row['id'] = $entity->id();
    // You probably want a few more properties here...
    return $row + parent::buildRow($entity);
  }

}
